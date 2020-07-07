#include "SynInferenceBrain.h"

namespace synchrono {
namespace agent {

SynInferenceBrain::SynInferenceBrain(int rank,
                                     std::shared_ptr<InferenceDriver> driver,
                                     ChVehicle& vehicle,
                                     std::shared_ptr<ChGPSSensor> gps,
                                     double control_frequency)
    : SynVehicleBrain(rank, driver, vehicle),
      m_receive_rank(rank + 1),
      m_inference_driver(driver),
      m_gps(gps),
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
    // Check if message is intented to be received by this rank
    // TODO: Could this be something handled at the MPI level?
    if (msg->GetID() != m_receive_rank)
        return;

    // Verify this message is a sensor type
    if (msg->GetType() == SynMessage::SENS) {
        // Get the state struct and access the gps data
        auto state = std::static_pointer_cast<SynSensorMessage::State>(msg->GetState());
        auto data = state->GetBufferAsType<GPSData>()[0];

        // Set the gps position of the leader within the inference driver
        m_inference_driver->SetLeaderGPSPosition(ChVector<>(data.Latitude, data.Longitude, data.Altitude));
    }
}

/// Generate vector of SynMessage's to send
void SynInferenceBrain::GenerateMessagesToSend(std::vector<SynMessage*>& messages) {
    if (m_gps == nullptr)
        return;

    // Get the gps data buffer pointer
    auto ptr = m_gps->GetMostRecentBuffer<UserGPSBufferPtr>();
    if (ptr->Buffer) {
        // Create the SensorBuffer for the message state
        auto buffer = chrono_types::make_shared<SynSensorMessage::SensorBuffer>(ptr->Width, ptr->Height,
                                                                                ptr->Buffer.get(), sizeof(GPSData));
        // Create the message state
        auto state = chrono_types::make_shared<SynSensorMessage::State>(0, SynSensorMessage::State::GPS, buffer);
        // Add message to the message vector
        messages.push_back(new SynSensorMessage(m_rank, state));
    }
}

}  // namespace agent
}  // namespace synchrono
