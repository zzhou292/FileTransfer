#include "InferenceDriver.h"

#include "chrono_sensor/ChSensor.h"
#include "chrono_sensor/utils/CudaMallocHelper.h"
#include "chrono_sensor/cuda/nn_prep.cuh"

#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace nvonnxparser;
using namespace nvinfer1;

InferenceDriver::InferenceDriver(ChVehicle& vehicle, std::string file_name, std::shared_ptr<ChCameraSensor> camera)
    : ChDriver(vehicle), m_file_name(file_name), m_camera(camera) {
    m_output_targets = std::vector<float>(2);  // 2 outputs steering, throttle
    m_throttle = 0;
    m_steering = 0;
    m_braking = 0;
    m_output_targets[0] = m_throttle;
    m_output_targets[1] = m_steering;
    m_dT = 0.01;
    m_dS = 0.005;
    m_dB = 0.0166;
    m_inputs.resize(6);
    for (auto input : m_inputs)
        input = 0;
}

void InferenceDriver::Initialize() {
    Logger l(NONE);  // NONE, PARTIAL,ALL
    // create inference builder
    auto builder = std::unique_ptr<IBuilder, TRTDestroyer>(createInferBuilder(l));
    if (!builder) {
        throw std::runtime_error("Could not create inference builder");
    }
    builder->setMaxBatchSize(1);

    // create network definition
    const auto explicitBatch = 1U << static_cast<uint32_t>(NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
    auto network = std::unique_ptr<INetworkDefinition, TRTDestroyer>(builder->createNetworkV2(explicitBatch));

    if (!network) {
        throw std::runtime_error("Could not create network definition");
    }

    // create builder configuration
    auto config = std::unique_ptr<IBuilderConfig, TRTDestroyer>(builder->createBuilderConfig());
    if (!config) {
        throw std::runtime_error("Could not create builder configuration");
    }

    // create UFF parser
    auto parser = std::unique_ptr<IParser, TRTDestroyer>(createParser(*network, l));
    if (!parser) {
        throw std::runtime_error("Could not create ONNX parser");
    }

    // parse the given network from the onnx model file
    parser->parseFromFile(m_file_name.c_str(), NONE);

    std::cout << "file: " << m_file_name.c_str() << std::endl;

    // create inference engine
    m_inference_engine = std::unique_ptr<ICudaEngine, TRTDestroyer>(builder->buildCudaEngine(*network));
    if (!m_inference_engine) {
        throw std::runtime_error("Could not create ONNX inference engine");
    }

    m_inference_context =
        std::unique_ptr<IExecutionContext, TRTDestroyer>(m_inference_engine->createExecutionContext());
    if (!m_inference_context) {
        throw std::runtime_error("Could not create ONNX inference context");
    }

    // SPECIFIC TO THIS MULTISENSOR SETUP
    double h = 45;
    double w = 80;
    img_gpu = std::shared_ptr<uint8_t>(
        cudaMallocHelper<uint8_t>(w * h * 4 * sizeof(uint8_t)),
        cudaFreeHelper<uint8_t>);

    m_process_buffers = std::vector<void*>(3);
    in1 = std::shared_ptr<float>(
        cudaMallocHelper<float>(w * h * 3 * sizeof(float)),
        cudaFreeHelper<float>);
    m_process_buffers[0] = in1.get();

    in2 = std::shared_ptr<float>(cudaMallocHelper<float>(6 * sizeof(float)), cudaFreeHelper<float>);
    m_process_buffers[1] = in2.get();

    out = std::shared_ptr<float>(cudaMallocHelper<float>(2 * sizeof(float)), cudaFreeHelper<float>);
    m_process_buffers[2] = out.get();
}

void InferenceDriver::Synchronize(double time) {
    auto pos = m_vehicle.GetChassisBody()->GetPos();
    auto vel = m_vehicle.GetChassisBody()->GetFrame_REF_to_abs().GetPos_dt();
    m_inputs = {(float)m_goal.x(), (float)m_goal.y(), (float)pos.x(), (float)pos.y(), (float)vel.x(), (float)vel.y()};

    double dist = (pos - m_goal).Length();
    if (dist < 5.0) {
      std::cout << "Dist :: " << dist << std::endl;
      std::cout << "Successful!!" << std::endl;
      exit(0);
    }

    auto camera_data = m_camera->GetMostRecentBuffer<UserRGBA8BufferPtr>();
    if (camera_data->Buffer) {
        // 1. copy data into structures on img_gpu
        cudaMemcpy(img_gpu.get(), camera_data->Buffer.get(), camera_data->Width * camera_data->Height * 4,
                   cudaMemcpyHostToDevice);
        cudaMemcpy(m_process_buffers[1], m_inputs.data(), 6 * sizeof(float), cudaMemcpyHostToDevice);

        // 2. preprocess data on gpu
        // slice out first three channels. Channel first is handled in network, slicing is not!
        preprocess_RGBA8_to_FLOAT3(img_gpu.get(), m_process_buffers[0], camera_data->Height, camera_data->Width);

        // std::vector<float> test(camera_data->Height * camera_data->Width * 3);
        // cudaMemcpy(test.data(), m_process_buffers[0], camera_data->Height * camera_data->Width * 3 * sizeof(float),
        // cudaMemcpyDeviceToHost); std::cout << "Before: " << (int)camera_data->Buffer[0].R << ", After: " << test[0]
        // << std::endl; cv::Mat mat2(camera_data->Height, camera_data->Width, CV_32FC3,  test.data()); cv::imwrite(
        // "image.png", mat2 ); exit(-1);

        // 3. perform inference
        m_inference_context->executeV2(m_process_buffers.data());

        // 4. move prediction back to host
        cudaMemcpy(m_output_targets.data(), m_process_buffers[2], 2 * sizeof(float), cudaMemcpyDeviceToHost);

        // 5. set steering, throttle, braking appropriately
        m_target_steering = m_output_targets[0];
        m_target_throttle = m_output_targets[1];
        // std::cout << "Action :: " << m_target_steering << ", " << m_target_throttle << std::endl;
    }
}

void InferenceDriver::Advance(double step) {
    // std::cout << "First :: " << m_steering << ", " << m_throttle << ", " << m_braking << std::endl;
    double next_steering = m_target_steering;
    double next_throttle = m_target_throttle;
    double next_braking = m_target_braking;
    ChClampValue(next_steering, m_steering - m_dS, m_steering + m_dS);
    if (next_throttle > 0) {
        next_throttle = abs(next_throttle);
        next_braking = 0;
        ChClampValue(next_throttle, m_throttle - m_dT, m_throttle + m_dT);
        ChClampValue(next_braking, m_braking - m_dB, m_braking + m_dB);
    } else {
        next_braking = abs(next_throttle);
        next_throttle = 0;
        ChClampValue(next_braking, m_braking - m_dB, m_braking + m_dB);
        ChClampValue(next_throttle, m_throttle - m_dT, m_throttle + m_dT);
    }
    m_steering = next_steering;
    m_throttle = next_throttle;
    m_braking = next_braking;
    // m_throttle = std::max(m_throttle - m_dT, std::min(m_target_throttle, m_throttle + m_dT));
    // m_steering = std::max(m_steering - m_dS, std::min(m_target_steering, m_steering + m_dS));
    // m_braking = std::max(m_braking - m_dB, std::min(m_target_braking, m_braking + m_dB));
    // std::cout << "Second :: " << m_steering << ", " << m_throttle << ", " << m_braking << std::endl;
}
