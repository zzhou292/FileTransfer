#ifndef SYN_TRAFFIC_LIGHT_BRAIN_H
#define SYN_TRAFFIC_LIGHT_BRAIN_H

#include "synchrono/SynApi.h"
#include "synchrono/SynBrain.h"

#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

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

class SYN_API TrafficLightBrain : public SynBrain {
  public:
    TrafficLightBrain(int id) : SynBrain(id) {}
    ~TrafficLightBrain() {}

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg);
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYN_TRAFFIC_LIGHT_BRAIN_H
