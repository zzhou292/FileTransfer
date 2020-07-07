#ifndef TRACK_ST_BRAIN_H
#define TRACK_ST_BRAIN_H

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"

#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

#include "chrono_vehicle/driver/ChPathFollowerACCDriver.h"
#include "chrono/core/ChVector.h"
#include "chrono_thirdparty/filesystem/path.h"

#ifdef SENSOR
#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/ChLidarSensor.h"
#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterVisualizePointCloud.h"
#include "chrono_sensor/filters/ChFilterPCfromDepth.h"
#include "chrono_sensor/filters/ChFilterSave.h"
using namespace chrono::sensor;
#endif

using namespace synchrono;

using namespace chrono;

namespace synchrono {
namespace agent {

class SYN_API TBrain : public SynVehicleBrain {
  public:
    TBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
#ifdef SENSOR
    TBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle, std::shared_ptr<ChLidarSensor> lidar);
#endif
    ~TBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg);

  private:
  

#ifdef SENSOR
    std::shared_ptr<ChSensorManager> m_manager;
    std::shared_ptr<ChLidarSensor> m_lidar;
    std::shared_ptr<ChCameraSensor> m_camera;

    bool m_save_data;
    std::string m_cam_data_path;
    std::string m_intersection_cam_data_path;

    double m_lidar_intensity_epsilon;
    UserDIBufferPtr m_recent_lidar_data;
#endif
};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
