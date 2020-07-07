#include "InferenceDriver.h"

InferenceDriver::InferenceDriver(ChVehicle& vehicle,
                                 std::string file_name,
                                 std::shared_ptr<ChCameraSensor> camera,
                                 std::shared_ptr<ChGPSSensor> agent_gps)
    : ChDriver(vehicle),
      m_vehicle(vehicle),
      m_file_name(file_name),
      m_camera(camera),
      m_agent_gps(agent_gps),
      m_log_csv(" ") {
    m_output_targets = std::vector<float>(2);  // 2 outputs steering, throttle
    m_throttle = 0;
    m_steering = 0;
    m_braking = 0;
    m_output_targets[0] = m_steering;
    m_output_targets[1] = m_throttle;
    m_dT = 0.01;
    m_dS = 0.005;
    m_dB = 0.0166;
    m_gps_input.resize(4);
    m_gps_input[0] = 0;
    m_gps_input[1] = 0;
    m_gps_input[2] = 0;
    m_gps_input[3] = 0;
    m_origin = ChVector<>(-89.400, 43.070, 260.0);
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
        cudaMallocHelper<float>(w * h * 4 * sizeof(float)),
        cudaFreeHelper<float>);
    m_process_buffers[0] = in1.get();

    in2 = std::shared_ptr<float>(cudaMallocHelper<float>(m_gps_input.size() * sizeof(float)), cudaFreeHelper<float>);
    m_process_buffers[1] = in2.get();

    out = std::shared_ptr<float>(cudaMallocHelper<float>(2 * sizeof(float)), cudaFreeHelper<float>);
    m_process_buffers[2] = out.get();
}

void InferenceDriver::Synchronize(double time) {
    // Get most recent GPS reading
    auto agent_gps_data = m_agent_gps->GetMostRecentBuffer<UserGPSBufferPtr>();
    if (agent_gps_data->Buffer) {
        // Calculate GPS input
        // Input is:
        // 1. Difference in latitude values of leader and follower * 100000
        // 2. Difference in longitude values of leader and follower * 100000
        // 3. Heading of the follower
        // 4. Change in distance between the leader and follower * control frequency [Hz]
        int f = 100000;
        m_gps_input[0] = (m_leader_coord.x() - agent_gps_data->Buffer[0].Latitude) * f;
        m_gps_input[1] = (m_leader_coord.y() - agent_gps_data->Buffer[0].Longitude) * f;
        m_gps_input[2] = m_vehicle.GetChassisBody()->GetRot().Q_to_Euler123().z();

        // Get position in the cartesian coordinate frame of the leader from the gps coordinate
        // TODO: Why do I have to flip the lat and long
        ChVector<> leader_pos(m_leader_coord.y(), m_leader_coord.x(), m_leader_coord.z());
        GPS2Cartesian(leader_pos, m_origin);
        // Calculate the distance from the leader to the follower
        double dist = (m_vehicle.GetChassisBody()->GetPos() - leader_pos).Length();
        if (m_lead_dist == 0)
            m_lead_dist = dist;
        m_gps_input[3] = (m_lead_dist - dist) * m_control_frequency;
        m_lead_dist = dist;
    }

    auto camera_data = m_camera->GetMostRecentBuffer<UserRGBA8BufferPtr>();
    if (camera_data->Buffer) {
        // 1. copy data into structures on img_gpu
        cudaMemcpy(img_gpu.get(), camera_data->Buffer.get(),
                   camera_data->Width * camera_data->Height * sizeof(PixelRGBA8), cudaMemcpyHostToDevice);
        cudaMemcpy(m_process_buffers[1], m_gps_input.data(), m_gps_input.size() * sizeof(float),
                   cudaMemcpyHostToDevice);

        // 2. preprocess data on gpu
        // slice out first three channels. Channel first is handled in network, slicing is not!
        preprocess_RGBA8_to_FLOAT3(img_gpu.get(), m_process_buffers[0], camera_data->Width, camera_data->Height);

        // 3. perform inference
        m_inference_context->executeV2(m_process_buffers.data());

        // 4. move prediction back to host
        cudaMemcpy(m_output_targets.data(), m_process_buffers[2], m_output_targets.size() * sizeof(float),
                   cudaMemcpyDeviceToHost);

        // 5. set steering, throttle, braking appropriately
        m_target_steering = m_output_targets[0];
        m_target_throttle = m_output_targets[1];
        // std::cout << "Action :: " << m_target_steering << ", " << m_target_throttle << std::endl;
    }
}

void InferenceDriver::Advance(double step) {
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
}
