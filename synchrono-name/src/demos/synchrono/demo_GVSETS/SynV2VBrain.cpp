#include "SynV2VBrain.h"

namespace synchrono {
namespace agent {

SynV2VBrain::SynV2VBrain(int rank,
                         std::shared_ptr<ChDriver> driver,
                         ChVehicle& vehicle,
                         std::shared_ptr<ChGPSSensor> gps)
    : SynVehicleBrain(rank, driver, vehicle), m_receive_rank(rank + 1), m_gps(gps), m_origin(-89.400, 43.070, 260.0) {
    m_box.GetSize() = ChVector<>(2, 2.5, 0);
}

// https://math.stackexchange.com/a/190373
bool SynV2VBrain::LeaderIsClose() {
    *m_box.GetRotm() = m_vehicle.GetChassisBody()->GetA();
    m_box.GetPos() = m_vehicle.GetChassisBody()->GetPos();
    // P5 through P8 are at -z
    // P6          P5
    //
    //       +
    //
    // P7          P8
    ChVector<> A = m_box.GetP8();
    ChVector<> B = m_box.GetP7();
    ChVector<> C = m_box.GetP6();
    ChVector<> M = m_leader_pos;
    ChVector<> AB = A - B;
    ChVector<> AM = A - M;
    ChVector<> BC = B - C;
    ChVector<> BM = B - M;

    return 0 <= (AB ^ AM) && (AB ^ AM) <= (AB ^ AB) && 0 < (BC ^ BM) && (BC ^ BM) <= (BC ^ BC);
}

void SynV2VBrain::Advance(double step) {
    // if (LeaderIsClose()) {
    //     std::cout << "Test" << std::endl;
    //     std::static_pointer_cast<ChPathFollowerACCDriver>(m_driver)->SetCurrentDistance(.5);
    // }
    m_driver->Advance(step);
}

void SynV2VBrain::ProcessMessage(SynMessage* msg) {
    // Check if message is intented to be received by this rank
    // TODO: Could this be something handled at the MPI level?
    if (msg->GetID() != m_receive_rank)
        return;

    // Verify this message is a sensor type
    if (msg->GetType() == SynMessage::SENS) {
        // Get the state struct and access the gps data
        auto state = std::static_pointer_cast<SynSensorMessage::State>(msg->GetState());
        auto data = state->GetBufferAsType<GPSData>()[0];

        // Set member variable for the ACC driver
        m_leader_pos = ChVector<>(data.Latitude, data.Longitude, data.Altitude);

        // Set the gps position of the leader within the inference driver
        if (std::shared_ptr<InferenceDriver> driver = std::dynamic_pointer_cast<InferenceDriver>(m_driver)) {
            driver->SetLeaderGPSPosition(m_leader_pos);  // Doesn't get passed as reference, so can change it
        }
        // Get position in the cartesian coordinate frame of the leader from the gps coordinate
        // TODO: Why do I have to flip the lat and long
        m_leader_pos = ChVector<>(m_leader_pos.y(), m_leader_pos.x(), m_leader_pos.z());
        GPS2Cartesian(m_leader_pos, m_origin);
    }
}

/// Generate vector of SynMessage's to send
void SynV2VBrain::GenerateMessagesToSend(std::vector<SynMessage*>& messages) {
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
