#include "synchrono/agent/SynSedanAgent.h"

namespace synchrono {
namespace agent {

SynSedanAgent::SynSedanAgent(int rank, ChSystem* system) : SynVehicleAgent(rank, SynVehicleAgent::SEDAN, system) {
    if (system) {
        m_sedan = chrono_types::make_shared<Sedan>(system);
        m_system = system;
    } else {
        m_sedan = chrono_types::make_shared<Sedan>();
        m_owns_system = true;
    }

    m_wheel_list.resize(4);

    // Set files for visualization
    m_chassis_vis_file = "sedan/sedan_chassis_vis.obj";
    m_wheel_vis_file = "sedan/sedan_rim.obj";
    m_tire_vis_file = "sedan/sedan_tire.obj";
}

void SynSedanAgent::InitializeVehicle(const ChCoordsys<>& pose) {
    // Setup the Sedan object that will represent this sedan
    m_sedan->SetContactMethod(ChContactMethod::SMC);
    m_sedan->SetChassisCollisionType(ChassisCollisionType::NONE);
    m_sedan->SetChassisFixed(false);
    m_sedan->SetInitPosition(pose);
    m_sedan->SetTireType(TireModelType::RIGID);
    m_sedan->SetTireStepSize(m_step_size);
    m_sedan->Initialize();

    m_sedan->SetChassisVisualizationType(VisualizationType::MESH);
    m_sedan->SetSuspensionVisualizationType(VisualizationType::NONE);
    m_sedan->SetSteeringVisualizationType(VisualizationType::NONE);
    m_sedan->SetWheelVisualizationType(VisualizationType::MESH);
    m_sedan->SetTireVisualizationType(VisualizationType::MESH);

    if (m_owns_system)
        m_system = m_sedan->GetSystem();
}

}  // namespace agent
}  // namespace synchrono
