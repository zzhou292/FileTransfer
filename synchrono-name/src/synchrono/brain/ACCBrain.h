#ifndef PARK_ST_BRAIN_H
#define PARK_ST_BRAIN_H

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

class SYN_API ACCBrain : public SynVehicleBrain {
  public:
    ACCBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
#ifdef SENSOR
    ACCBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle, std::shared_ptr<ChLidarSensor> lidar);
#endif
    ~ACCBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg);

    void setLane(int lane) { m_lane = lane; }

    bool m_sen_init_called;

    void SetNearestVehicleDistance(float dist) { m_nearest_vehicle = dist; }

  private:
    // Hard coded current lane. Change later?
    int m_lane = 0;
    int m_light_color = 0;      // Starts red (0: green, 1: yellow, 2: yellow)
    bool m_inside_box = false;  // Is vehicle inside stopping box area
    double m_dist = 1000;       // Distance to point to stop at light

    double DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2);
    bool IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w);

    int m_nearest_vehicle;

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
