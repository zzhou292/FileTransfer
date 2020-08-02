#include "synchrono/brain/GeneralBrain.h"

namespace synchrono {
namespace agent {

GeneralBrain::GeneralBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleBrain(rank, driver, vehicle) {
    m_nearest_vehicle = 1000;

    m_rank = rank;
}

GeneralBrain::~GeneralBrain() {}

void GeneralBrain::Synchronize(double time) {
    for (int i = 0; i < moduleVecLength(); i++){
        compArr[i]->Synchronize(time);
    }
}

void GeneralBrain::Advance(double step) {
    for (int i = 0; i < moduleVecLength(); i++){
        
        compArr[i]->Advance(step);
    }
}

void GeneralBrain::ProcessMessage(SynMessage* msg, int sender_rank) {
    for (int i = 0; i < moduleVecLength(); i++){
        compArr[i]->ProcessMessage(msg,sender_rank);
    }
}



}  // namespace agent
}  // namespace synchrono
