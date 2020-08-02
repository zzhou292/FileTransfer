#ifndef TRACK_ST_BRAIN_H
#define TRACK_ST_BRAIN_H

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

class SYN_API TrackingBrain : public SynVehicleBrain {
  public:
    TrackingBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
    ~TrackingBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg, int sender_rank);

    void setLane(int lane) { m_lane = lane; }
    void trackLoc(SynMessage* msg, int sender_rank,chrono::Vector Sen);

    virtual void updateMyLoc(chrono::Vector);

    //Helper function for circular collision detection
    void checkDistanceCircle(ChVector<> pos1,ChVector<> pos2, double radius,int sender_rank);
  
    void checkDistanceRecCollision(ChVector<> pos1,ChVector<> pos2, double shortRecSide,
    double longRecSide,int sender_rank);

    void enableLocDisplay();
    void disableLocDisplay();

    void actionUsrRec(){
      (*usrDefinedActionRec)();}

    void actionUsrCir(){
      (*usrDefinedActionCir)();}

    void addActionUsrRec(void (*ptr)()){
      usrDefinedActionRec = ptr;
    }

    void addActionUsrCir(void (*ptr)()){
      usrDefinedActionCir = ptr;
    }
    
    
  private:




    int m_lane = 0;
    int m_light_color = 0;      // Starts red (0: green, 1: yellow, 2: yellow)
    bool m_inside_box = false;  // Is vehicle inside stopping box area
    double m_dist = 1000;       // Distance to point to stop at light

    bool enableDis = false;
    chrono::Vector myLoc;

    int m_nearest_vehicle;

    double DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2);
    bool IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w);

    //usr defined collision action
    void (*usrDefinedActionRec)() = NULL;
    void (*usrDefinedActionCir)() = NULL;


    std::vector<std::shared_ptr<SynVehicleComponent>> compArr;

};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
