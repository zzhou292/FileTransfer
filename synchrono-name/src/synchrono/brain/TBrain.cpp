#include "synchrono/brain/TBrain.h"

namespace synchrono {
namespace agent {

TBrain::TBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleBrain(rank, driver, vehicle) {

}

#ifdef SENSOR
TBrain::TBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle, std::shared_ptr<ChLidarSensor> lidar)
    : SynVehicleBrain(rank, driver, vehicle) {
    TBrain(rank, driver, vehicle);
    m_lidar = lidar;
}
#endif

TBrain::~TBrain() {}

void TBrain::Synchronize(double time) {


#ifdef SENSOR


#endif
    

    // ChDriver::Inputs driver_inputs = m_driver->GetInputs();
    // // driver_inputs.m_braking = 1;
    // // driver_inputs.m_throttle = .05;
    // // driver_inputs.m_steering = 0;
    // std::cout << "\r\nsteering: " << driver_inputs.m_steering;
    // std::cout << "\r\nthrottle: " << driver_inputs.m_throttle;
    // std::cout << "\r\nbraking: " << driver_inputs.m_braking;
    // std::cout << std::endl;
}

void TBrain::Advance(double step) {

}

void TBrain::ProcessMessage(SynMessage* msg) {
    
}



}  // namespace agent
}  // namespace synchrono
