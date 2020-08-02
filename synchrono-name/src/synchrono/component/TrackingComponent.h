#ifndef TRACK_ST_TrackingComponent_H
#define TRACK_ST_TrackingComponent_H

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

class SYN_API TrackingComponent : public SynVehicleComponent {
  public:
    TrackingComponent(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle);
    ~TrackingComponent();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg, int sender_rank);

    void setLane(int lane) { m_lane = lane; }
    void trackLoc(SynMessage* msg, int sender_rank,chrono::Vector Sen);

    virtual void updateMyLoc(chrono::Vector);

    //Helper function for circular collision detection
    void checkDistanceCircle(ChVector<> pos1,ChVector<> pos2, double radius,int sender_rank);
  
    void checkDistanceRec(ChVector<> pos1,ChVector<> pos2, double shortRecSide,
    double longRecSide,int sender_rank);

    void enableLocDisplay();
    void disableLocDisplay();

    void actionUsrRec(){
      for(int i=0 ; i<usrDefinedActionRec.size() ; i++){
        (*usrDefinedActionRec[i])();
      }
    }

    void actionUsrCir(){
      for(int i=0 ; i<usrDefinedActionCir.size() ; i++){
        (*usrDefinedActionCir[i])();
      }
    }

    void addActionUsrRec(void (*ptr)()){
      usrDefinedActionRec.push_back(ptr);
    }

    void addActionUsrCir(void (*ptr)()){
      usrDefinedActionCir.push_back(ptr);
    }

    void addTargetRank(int target){
      target_rank.push_back(target);
    }

    void printTargetRank(){

      if(target_rank.size()==0){
        std::cout<<"WARNING: No target rank has been set, will detect on all ranks"<<std::endl;
      }

      for(int i=0; i<target_rank.size() ; i++){
        std::cout<<"target_Rank: "<<std::endl;
        std::cout<<target_rank[i]<<std::endl;
      }
    }

    void removeTargetRank(int rm_target){
      for(int i=0; i<target_rank.size();i++){
        if(rm_target == target_rank[i]){
          target_rank.erase(target_rank.begin()+i);
        }
      }
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

    //std::vector<void (*)()> *usrDefinedActionRec;
    //std::vector<void (*)()> *usrDefinedActionCir;

    std::vector<void(*) ()> usrDefinedActionRec;
    std::vector<void(*) ()> usrDefinedActionCir;

    std::vector<int> target_rank;


};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
