#ifndef SYN_INFERENCE_BRAIN_H
#define SYN_INFERENCE_BRAIN_H

#include "synchrono/SynVehicleBrain.h"

#include "InferenceDriver.h"

#include "synchrono/flatbuffer/message/SynSensorMessage.h"

#include "chrono_sensor/ChGPSSensor.h"

namespace synchrono {
namespace agent {

class SYN_API SynInferenceBrain : public SynVehicleBrain {
  public:
    /// Constructor
    SynInferenceBrain(int rank,
                      std::shared_ptr<InferenceDriver> driver,
                      ChVehicle& vehicle,
                      std::shared_ptr<ChGPSSensor> gps,
                      double control_frequency = 5);

    /// Destructor
    ~SynInferenceBrain() {}

    /// Synchronoize this brain with the rest of the simulation.
    virtual void Synchronize(double time);

    /// Advance the state of this brain until brain time syncs with passed time.
    // virtual void Advance(double step);

    /// Process an incoming message
    virtual void ProcessMessage(SynMessage* msg);

    /// Generate vector of SynMessage's to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) override;

    /// Set handle to gps
    void SetGPS(std::shared_ptr<ChGPSSensor> gps) { m_gps = gps; }

  private:
    int m_receive_rank;  ///< rank which inference brain should listen to incoming messages

    double m_control_frequency;  ///< frequency at which inference updates at
    double m_last_update;        ///< last time inference occured

    std::shared_ptr<InferenceDriver> m_inference_driver;  ///< handle to the inference Chrono driver
    std::shared_ptr<ChGPSSensor> m_gps;                   ///< Handle to the GPS sensor
};

}  // namespace agent
}  // namespace synchrono

#endif
