#ifndef PARK_ST_GeneralBrain_H
#define PARK_ST_GeneralBrain_H

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"
#include "synchrono/SynVehicleComponent.h"
#include "synchrono/component/TrackingComponent.h"
#include "synchrono/component/ACCComponent.h"

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

class SYN_API GeneralBrain : public SynVehicleBrain {
  public:
    GeneralBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
#ifdef SENSOR
    GeneralBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle, std::shared_ptr<ChLidarSensor> lidar);
#endif
    ~GeneralBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg, int sender_rank);

    void setLane(int lane) { m_lane = lane; }

    bool m_sen_init_called;

    void SetNearestVehicleDistance(float dist) { m_nearest_vehicle = dist; }

    void updateMyLoc(chrono::Vector){};

    // add module to the GeneralBrain
    void addModule(std::shared_ptr<SynVehicleComponent> module){compArr.push_back(module);}

    // return the number of module in the GeneralBrain
    int moduleVecLength(){return compArr.size();}
    
  private:
    // Hard coded current lane. Change later?
    int m_lane = 0;
    int m_light_color = 0;      // Starts red (0: green, 1: yellow, 2: yellow)
    bool m_inside_box = false;  // Is vehicle inside stopping box area
    double m_dist = 1000;       // Distance to point to stop at light

  
    int m_nearest_vehicle;

    //?? CHANGE TO SynVehicleComponent Type
    std::vector<std::shared_ptr<SynVehicleComponent>> compArr;

    
};

}  // namespace agent
}  // namespace synchrono

#endif  
