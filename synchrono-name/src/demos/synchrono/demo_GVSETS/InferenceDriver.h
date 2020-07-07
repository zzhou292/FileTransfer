#ifndef INFERENCE_DRIVER_H
#define INFERENCE_DRIVER_H

#include "synchrono/SynApi.h"

#include "chrono_vehicle/ChApiVehicle.h"
#include "chrono_vehicle/ChDriver.h"
#include "chrono_vehicle/ChVehicle.h"

#include <NvInfer.h>
#include <NvOnnxParser.h>
#include <NvUtils.h>
#include <cuda_runtime_api.h>

#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/ChGPSSensor.h"
#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/utils/CudaMallocHelper.h"
#include "chrono_sensor/cuda/nn_prep.cuh"
#include "chrono_sensor/tensorrt/ChTRTUtils.h"

#include "chrono/utils/ChUtilsInputOutput.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::sensor;

using namespace nvonnxparser;
using namespace nvinfer1;

class SYN_API InferenceDriver : public ChDriver {
  public:
    InferenceDriver(ChVehicle& vehicle,
                    std::string file_name,
                    std::shared_ptr<ChCameraSensor> camera,
                    std::shared_ptr<ChGPSSensor> agent_gps);

    ~InferenceDriver() {}

    /// Set the steering deltas
    void SetDeltas(float dS, float dT, float dB) {
        m_dS = dS;
        m_dT = dT;
        m_dB = dB;
    }

    /// Initialize the underlying TensorRT engine and prepare GPU to hold the model data
    virtual void Initialize();

    /// Perform the inference
    virtual void Synchronize(double time);

    /// Update the driver inputs
    virtual void Advance(double step);

    /// Sets the position of the leader coord
    void SetLeaderGPSPosition(ChVector<> coord) { m_leader_coord = coord; }

  private:
    std::string m_file_name;  ///< File name for the .onnx file

    std::unique_ptr<nvinfer1::ICudaEngine, TRTDestroyer> m_inference_engine;         ///< TensorRT inference engine
    std::unique_ptr<nvinfer1::IExecutionContext, TRTDestroyer> m_inference_context;  ///< TensorRT inference context

    // driving deltas
    float m_dS;  ///< Steering delta
    float m_dT;  ///< Throttle delta
    float m_dB;  ///< Braking delta

    // Target driver inputs
    double m_target_steering = 0;  ///< Target steering value
    double m_target_throttle = 0;  ///< Target throttle value
    double m_target_braking = 0;   ///< Target braking value

    ChVector<> m_leader_coord;  ///< GPS coordinate of leader. Set by the V2VBrain
    ChVector<> m_origin;        ///< GPS origin for coordinate conversions to get cartesian position of leader

    double m_control_frequency = 5;  ///< Frequency of which inference should happen in Hz
    double m_lead_dist = 0;          ///< Saved distance between the leader and the follower

    // handles to input sensors
    std::shared_ptr<ChCameraSensor> m_camera;  ///< Handle to the input camera
    std::shared_ptr<ChGPSSensor> m_agent_gps;  ///< Handle to the input gps

    ChVehicle& m_vehicle;  ///< reference to the agents vehicle

    // references to in1,in2,out on gpu
    std::shared_ptr<uint8_t> img_gpu;  ///< handle to the uint8_t RGBA8 buffer on the gpu
    std::shared_ptr<float> in1;
    std::shared_ptr<float> in2;
    std::shared_ptr<float> out;
    std::vector<float> m_gps_input;        ///< GPS input to the model
    std::vector<void*> m_process_buffers;  ///< Buffer used on inference
    std::vector<float> m_output_targets;   ///< Output from the inference

    // for logging
    chrono::utils::CSV_writer m_log_csv;  ///< CSV writer for logging data
};

#endif
