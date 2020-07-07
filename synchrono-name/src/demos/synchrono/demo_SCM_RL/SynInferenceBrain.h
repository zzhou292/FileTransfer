#ifndef SYN_INFERENCE_BRAIN_H
#define SYN_INFERENCE_BRAIN_H

#include "synchrono/SynVehicleBrain.h"

#include "InferenceDriver.h"

#include "synchrono/flatbuffer/message/SynSensorMessage.h"

namespace synchrono {
namespace agent {

class SYN_API SynInferenceBrain : public SynVehicleBrain {
  public:
    /// Constructor
    SynInferenceBrain(int id,
                      std::shared_ptr<InferenceDriver> driver,
                      ChVehicle& vehicle,
                      double control_frequency = 5);

    /// Destructor
    ~SynInferenceBrain() {}

    /// Synchronoize this brain with the rest of the simulation.
    virtual void Synchronize(double time);

    /// Advance the state of this brain until brain time syncs with passed time.
    // virtual void Advance(double step);

    /// Process an incoming message
    virtual void ProcessMessage(SynMessage* msg);

  private:
    int m_id;  ///< agent rank

    double m_control_frequency;  ///< frequency at which inference updates at
    double m_last_update;        ///< last time inference occured

    std::shared_ptr<InferenceDriver> m_inference_driver;  ///< handle to the inference Chrono driver
};

}  // namespace agent
}  // namespace synchrono

#endif
