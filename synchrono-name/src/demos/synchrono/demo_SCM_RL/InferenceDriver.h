#ifndef INFERENCE_DRIVER_H
#define INFERENCE_DRIVER_H

#include <string>
#include <vector>
#include <memory>
#include "synchrono/SynApi.h"

#include "chrono_vehicle/ChApiVehicle.h"
#include "chrono_vehicle/ChDriver.h"
#include "chrono_vehicle/ChVehicle.h"

#include <NvInfer.h>
#include <NvOnnxParser.h>
#include <NvUtils.h>
#include <cuda_runtime_api.h>
#include "chrono_sensor/tensorrt/ChTRTUtils.h"

#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/ChGPSSensor.h"
#include "chrono_sensor/ChCameraSensor.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::sensor;

class SYN_API InferenceDriver : public ChDriver {
  public:
    InferenceDriver(ChVehicle& vehicle, std::string file_name, std::shared_ptr<ChCameraSensor> camera);

    ~InferenceDriver() {}

    void SetDeltas(float dS, float dT, float dB) {
        m_dS = dS;
        m_dT = dT;
        m_dB = dB;
    }

    virtual void Initialize();
    virtual void Synchronize(double time);
    virtual void Advance(double step);

    /// Sets the position of the leader coord
    void SetGoalPosition(ChVector<> goal) { m_goal = goal; }

  private:
    std::string m_file_name;

    std::unique_ptr<nvinfer1::ICudaEngine, TRTDestroyer> m_inference_engine;
    std::unique_ptr<nvinfer1::IExecutionContext, TRTDestroyer> m_inference_context;

    // driving deltas
    float m_dS;
    float m_dT;
    float m_dB;

    ChVector<> m_goal;

    double m_target_steering = 0;
    double m_target_throttle = 0;
    double m_target_braking = 0;

    // handles to input sensors
    std::shared_ptr<ChCameraSensor> m_camera;

    // references to in1,in2,out on gpu
    bool m_inputs_set = false;
    std::shared_ptr<uint8_t> img_gpu;
    std::shared_ptr<float> in1;
    std::shared_ptr<float> in2;
    std::shared_ptr<float> out;
    std::vector<void*> m_process_buffers;
    std::vector<float> m_output_targets;
    std::vector<float> m_inputs;
};

#endif
