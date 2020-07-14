#include "synchrono/brain/ACCBrain.h"

namespace synchrono {
namespace agent {

ACCBrain::ACCBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleBrain(rank, driver, vehicle) {
    m_nearest_vehicle = 1000;

    m_rank = rank;
}

//#ifdef SENSOR
//ACCBrain::ACCBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle, std::shared_ptr<ChLidarSensor> lidar)
//    : SynVehicleBrain(rank, driver, vehicle) {
 //   ACCBrain(rank, driver, vehicle);
 //   m_lidar = lidar;
//}
//#endif

ACCBrain::~ACCBrain() {}

void ACCBrain::Synchronize(double time) {
    double temp_range = 1000;
    if (false && GetRank() == 4) {
        std::string color = m_light_color == 0 ? "green" : (m_light_color == 1 ? "yellow" : "red");
        std::string box = m_inside_box ? "inside box" : "not inside box";
        std::cout << "Light is " << color << " and " << box << std::endl;
    }
    if (m_light_color == SynSPATMessage::RED && m_inside_box) {
        temp_range = temp_range < m_dist ? temp_range : m_dist;
    }
    if (false && GetRank() == 4) {
        std::cout << "Distance is: " << m_dist << std::endl;
    }

#ifdef SENSOR
    if (m_lidar) {
        // This only returns non-null at the lidar frequency, so only overwrite if it was non-null
        UserDIBufferPtr recent_lidar = m_lidar->GetMostRecentBuffer<UserDIBufferPtr>();

        if (recent_lidar->Buffer) {
            m_recent_lidar_data = recent_lidar;
        }

        float min_val = m_nearest_vehicle / 15.0;
        if (GetRank() == 5) {
            // std::cout << m_nearest_vehicle << std::endl;
        }

        if (GetRank() == 2 && m_nearest_vehicle != 1000) {
            // std::cout << m_nearest_vehicle << std::endl;
        }

        if (m_recent_lidar_data && m_recent_lidar_data->Buffer) {
            for (int i = 0; i < m_recent_lidar_data->Height; i++) {
                for (int j = 0; j < m_recent_lidar_data->Width; j++) {
                    double temp_range = m_recent_lidar_data->Buffer[i * m_recent_lidar_data->Width + j].range;
                    double temp_intensity = m_recent_lidar_data->Buffer[i * m_recent_lidar_data->Width + j].intensity;

                    if (temp_range < min_val && temp_intensity >= m_lidar_intensity_epsilon) {
                        min_val = temp_range;
                    }
                }
            }
        }

        temp_range = temp_range < min_val ? temp_range : min_val;
        // std::cout << "Range: " << temp_range << std::endl;
    }

#endif
    std::static_pointer_cast<ChPathFollowerACCDriver>(m_driver)->SetCurrentDistance(temp_range);

    // std::cout << "Range is: " << temp_range << std::endl;

    m_driver->Synchronize(time);

    // ChDriver::Inputs driver_inputs = m_driver->GetInputs();
    // // driver_inputs.m_braking = 1;
    // // driver_inputs.m_throttle = .05;
    // // driver_inputs.m_steering = 0;
    // std::cout << "\r\nsteering: " << driver_inputs.m_steering;
    // std::cout << "\r\nthrottle: " << driver_inputs.m_throttle;
    // std::cout << "\r\nbraking: " << driver_inputs.m_braking;
    // std::cout << std::endl;
}

void ACCBrain::Advance(double step) {
    m_driver->Advance(step);
}

void ACCBrain::ProcessMessage(SynMessage* msg, int sender_rank) {
     //std::cout<<"pos"<<m_vehicle.GetChassisBody()->GetPos()<<std::endl;
    switch (msg->GetType()) {
        case SynMessage::TRAFFIC_LIGHT: {
            // jht - Doesn't seem like we ever call delete, is this a memory leak?
            auto spat = std::static_pointer_cast<SynTrafficLightMessage::State>(msg->GetState())->spat;
            SynSPATMessage* spat_msg =
                new SynSPATMessage(m_rank, chrono_types::make_shared<SynSPATMessage::State>(spat));
            ProcessMessage(spat_msg,sender_rank);
            auto map = std::static_pointer_cast<SynTrafficLightMessage::State>(msg->GetState())->map;
            SynMAPMessage* map_msg = new SynMAPMessage(m_rank, chrono_types::make_shared<SynMAPMessage::State>(map));
            ProcessMessage(map_msg,sender_rank);
            break;
        }
        case SynMessage::SPAT: {
            // SPAT Processing
            auto spat_lanes = std::static_pointer_cast<SynSPATMessage::State>(msg->GetState())->lanes;
            // std::cout << "Processing Message ::" << std::endl;
            // for (auto lane : spat_lanes) {
            //     std::cout << lane.color << " ";
            // }
            // std::cout << std::endl;

            // std::cout << "Lane :: " << m_lane << std::endl;
            // std::cout << "Lane Color :: " << spat_lanes[m_lane].color << std::endl;

            m_light_color = spat_lanes[m_lane].color;
            break;
        }
        case SynMessage::MAP: {
            // MAP Processing
            auto map_lanes = std::static_pointer_cast<SynMAPMessage::State>(msg->GetState())->lanes;
            ChVector<> pos = m_vehicle.GetChassisBody()->GetPos();
           

            if (IsInsideBox(pos, map_lanes[m_lane].stopPos, map_lanes[m_lane].offsetPos, map_lanes[m_lane].width)) {
                m_inside_box = true;
            } else {
                m_inside_box = false;
            }
            m_dist = (map_lanes[m_lane].stopPos - pos).Length();

            break;
        }
    }
}

double ACCBrain::DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2) {
    // https://www.intmath.com/plane-analytic-geometry/perpendicular-distance-point-line.php
    double A = l1.y() - l2.y();
    double B = -(l1.x() - l2.x());
    double C = l1.x() * l2.y() - l1.y() * l2.x();
    double d = abs(A * p.x() + B * p.y() + C) / sqrt(A * A + B * B);
    return d;
}

bool ACCBrain::IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w) {
    double len = (sp - op).Length();
    // TODO :: Should be w / 2, but paths are too far away from the actual lanes

    return (pos - sp).Length() < len && (pos - op).Length() < len && DistanceToLine(pos, sp, op) < w;
}

}  // namespace agent
}  // namespace synchrono
