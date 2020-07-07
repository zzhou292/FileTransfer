#include "synchrono/agent/SynCityBusAgent.h"

namespace synchrono {
namespace agent {

SynCityBusAgent::SynCityBusAgent(int rank, ChSystem* system) : SynVehicleAgent(rank, SynVehicleAgent::CITYBUS, system) {
    if (system) {
        m_citybus = chrono_types::make_shared<CityBus>(system);
        m_system = system;
    } else {
        m_citybus = chrono_types::make_shared<CityBus>();
        m_owns_system = true;
    }

    m_wheel_list.resize(6);

    // Set visualization files
    m_chassis_vis_file = "citybus/CityBus_Vis.obj";
    m_wheel_vis_file = "citybus/CityBusRim.obj";
    m_tire_vis_file = "citybus/CityBusTire.obj";
}

void SynCityBusAgent::InitializeVehicle(const ChCoordsys<>& pose) {
    // Setup the Sedan object that will represent this sedan
    m_citybus->SetContactMethod(ChContactMethod::SMC);
    m_citybus->SetChassisCollisionType(ChassisCollisionType::NONE);
    m_citybus->SetChassisFixed(false);
    m_citybus->SetInitPosition(pose);
    m_citybus->SetTireType(TireModelType::RIGID);
    m_citybus->SetTireStepSize(m_step_size);
    m_citybus->Initialize();

    m_citybus->SetChassisVisualizationType(VisualizationType::MESH);
    m_citybus->SetSuspensionVisualizationType(VisualizationType::NONE);
    m_citybus->SetSteeringVisualizationType(VisualizationType::NONE);
    m_citybus->SetWheelVisualizationType(VisualizationType::MESH);
    m_citybus->SetTireVisualizationType(VisualizationType::MESH);

    if (m_owns_system)
        m_system = m_citybus->GetSystem();
}

}  // namespace agent
}  // namespace synchrono
