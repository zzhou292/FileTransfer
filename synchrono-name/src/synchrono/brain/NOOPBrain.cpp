#include "synchrono/brain/NOOPBrain.h"

namespace synchrono {
namespace agent {

NOOPBrain::NOOPBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleBrain(rank, driver, vehicle) {
}

NOOPBrain::~NOOPBrain() {}

void NOOPBrain::Synchronize(double time) {
    m_driver->Synchronize(time);
}

void NOOPBrain::Advance(double step) {
    m_driver->Advance(step);
}

void NOOPBrain::SetNearestVehicleDistance(float dist) {}

void NOOPBrain::ProcessMessage(SynMessage* msg) {
    switch (msg->GetType()) {
        case SynMessage::TRAFFIC_LIGHT: {
            auto spat = std::static_pointer_cast<SynTrafficLightMessage::State>(msg->GetState())->spat;
            SynSPATMessage* spat_msg = new SynSPATMessage(m_rank, chrono_types::make_shared<SynSPATMessage::State>(spat));
            ProcessMessage(spat_msg);
            auto map = std::static_pointer_cast<SynTrafficLightMessage::State>(msg->GetState())->map;
            SynMAPMessage* map_msg = new SynMAPMessage(m_rank, chrono_types::make_shared<SynMAPMessage::State>(map));
            ProcessMessage(map_msg);
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
            //
            // std::cout << "Lane :: " << m_lane << std::endl;
            // std::cout << "Lane Color :: " << spat_lanes[m_lane].color << std::endl;

            m_light_color = spat_lanes[m_lane].color;
            break;
        }
        case SynMessage::MAP: {
            // MAP Processing
            auto map_lanes = std::static_pointer_cast<SynMAPMessage::State>(msg->GetState())->lanes;
            ChVector<> pos = m_vehicle.GetChassisBody()->GetPos();
            m_inside_box = false;
            m_dist = (map_lanes[m_lane].stopPos - pos).Length();
            // std::cout << "Distance :: " << m_dist << std::endl;
            break;
        }
    }
}

}  // namespace agent
}  // namespace synchrono
