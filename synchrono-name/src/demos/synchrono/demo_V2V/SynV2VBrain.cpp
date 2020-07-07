#include "SynV2VBrain.h"

namespace synchrono {
namespace agent {

SynV2VBrain::SynV2VBrain(int rank,
                         std::shared_ptr<ChDriver> driver,
                         ChVehicle& vehicle,
                         std::shared_ptr<ChGPSSensor> gps,
                         std::shared_ptr<ChIMUSensor> imu)
    : SynVehicleBrain(rank, driver, vehicle), m_gps(gps), m_imu(imu) {}

void SynV2VBrain::ProcessMessage(SynMessage* msg) {
    if (msg->GetType() == SynMessage::SENS) {
        auto sensor_state = std::static_pointer_cast<SynSensorMessage::State>(msg->GetState());
        if (sensor_state->type == SynSensorMessage::State::GPS) {
            auto gps_data = sensor_state->GetBufferAsType<GPSData>()[0];
            std::cout << "Latitude :: " << gps_data.Latitude << std::endl;
        } else if (sensor_state->type == SynSensorMessage::State::IMU) {
            auto imu_data = sensor_state->GetBufferAsType<IMUData>()[0];
            std::cout << "Yaw :: " << imu_data.Yaw << std::endl;
        }
    }
}

/// Generate vector of SynMessage's to send
void SynV2VBrain::GenerateMessagesToSend(std::vector<SynMessage*>& messages) {
    if (m_gps != nullptr) {
        auto gps_buffer_ptr = m_gps->GetMostRecentBuffer<UserGPSBufferPtr>();
        if (gps_buffer_ptr->Buffer) {
            auto sensor_buffer = chrono_types::make_shared<SynSensorMessage::SensorBuffer>(
                gps_buffer_ptr->Width, gps_buffer_ptr->Height, gps_buffer_ptr->Buffer.get(), sizeof(GPSData));
            auto state = chrono_types::make_shared<SynSensorMessage::State>(0, SynSensorMessage::State::GPS, sensor_buffer);
            messages.push_back(new SynSensorMessage(m_rank, state));
        }
    }

    if (m_imu != nullptr) {
        auto imu_buffer_ptr = m_imu->GetMostRecentBuffer<UserIMUBufferPtr>();
        if (imu_buffer_ptr->Buffer) {
            auto sensor_buffer = chrono_types::make_shared<SynSensorMessage::SensorBuffer>(
                imu_buffer_ptr->Width, imu_buffer_ptr->Height, imu_buffer_ptr->Buffer.get(), sizeof(IMUData));
            auto state = chrono_types::make_shared<SynSensorMessage::State>(0, SynSensorMessage::State::IMU, sensor_buffer);
            messages.push_back(new SynSensorMessage(m_rank, state));
        }
    }
}

}  // namespace agent
}  // namespace synchrono
