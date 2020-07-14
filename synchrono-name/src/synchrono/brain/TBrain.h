#ifndef TRACK_ST_BRAIN_H
#define TRACK_ST_BRAIN_H

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"

#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

#include "chrono_vehicle/driver/ChPathFollowerACCDriver.h"
#include "chrono/core/ChVector.h"
#include "chrono_thirdparty/filesystem/path.h"

#include "synchrono/flatbuffer/message/SynVehicleMessage.h"

using namespace synchrono;

using namespace chrono;

namespace synchrono {
namespace agent {

class SYN_API TBrain : public SynVehicleBrain {
  public:
    TBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
    ~TBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg, int sender_rank);

    void setLane(int lane) { m_lane = lane; }
    void trackLoc(SynMessage* msg, int sender_rank,chrono::Vector Sen);

    virtual void updateMyLoc(chrono::Vector);

    
    void checkDistanceCircle(ChVector<> pos1,ChVector<> pos2);
    void checkDistanceCircleCollision(double distance);
    void checkDistanceRecCollision(ChVector<> pos1,ChVector<> pos2);
    
    
  private:




    int m_lane = 0;
    int m_light_color = 0;      // Starts red (0: green, 1: yellow, 2: yellow)
    bool m_inside_box = false;  // Is vehicle inside stopping box area
    double m_dist = 1000;       // Distance to point to stop at light
    chrono::Vector myLoc;

    int m_nearest_vehicle;

    double DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2);
    bool IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w);



};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
