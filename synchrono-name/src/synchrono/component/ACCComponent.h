#ifndef TRACK_ST_ACCComponent_H
#define TRACK_ST_ACCComponent_H

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"
#include "synchrono/SynVehicleComponent.h"
#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

#include "chrono_vehicle/driver/ChPathFollowerACCDriver.h"
#include "chrono/core/ChVector.h"
#include "chrono_thirdparty/filesystem/path.h"

#include "synchrono/flatbuffer/message/SynVehicleMessage.h"

using namespace synchrono;

using namespace chrono;

namespace synchrono {
namespace agent {

class SYN_API ACCComponent : public SynVehicleComponent {
  public:
    ACCComponent(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
    ~ACCComponent();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg, int sender_rank);

    void setLane(int lane) { m_lane = lane; }

    bool m_sen_init_called;

    void SetNearestVehicleDistance(float dist) { m_nearest_vehicle = dist; }

    virtual void updateMyLoc(chrono::Vector){};
    
    
  private:
    // Hard coded current lane. Change later?
    int m_lane = 0;
    int m_light_color = 0;      // Starts red (0: green, 1: yellow, 2: yellow)
    bool m_inside_box = false;  // Is vehicle inside stopping box area
    double m_dist = 1000;       // Distance to point to stop at light

    double DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2);
    bool IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w);

    int m_nearest_vehicle;



};

}  // namespace agent
}  // namespace synchrono

#endif 
