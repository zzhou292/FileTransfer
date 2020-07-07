#ifndef NOOP_BRAIN_H
#define NOOP_BRAIN_H

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"

#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

#include "chrono_vehicle/driver/ChPathFollowerACCDriver.h"
#include "chrono/core/ChVector.h"

using namespace synchrono;

using namespace chrono;

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

namespace synchrono {
namespace agent {

class SYN_API NOOPBrain : public SynVehicleBrain {
  public:
    NOOPBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
    ~NOOPBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg);

    void SetNearestVehicleDistance(float dist);

  private:
    // Hard coded current lane. Change later?
    int m_lane = 0;
    int m_light_color = 0;      // Starts red (0: red, 1: yellow, 2: green)
    bool m_inside_box = false;  // Is vehicle inside stopping box area
    double m_dist = 1000;       // Distance to point to stop at light
};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
