#include "SynInferenceBrain.h"

namespace synchrono {
namespace agent {

SynInferenceBrain::SynInferenceBrain(int id,
                                     std::shared_ptr<InferenceDriver> driver,
                                     ChVehicle& vehicle,
                                     double control_frequency)
    : SynVehicleBrain(id, driver, vehicle),
      m_inference_driver(driver),
      m_control_frequency(control_frequency),
      m_last_update(-1) {}

void SynInferenceBrain::Synchronize(double time) {
    // synchronize at prescribed control frequency
    if (time - m_last_update >= 1 / m_control_frequency) {
        m_inference_driver->Synchronize(time);
        m_last_update = time;
    }
}

void SynInferenceBrain::ProcessMessage(SynMessage* msg) {
}

}  // namespace agent
}  // namespace synchrono
