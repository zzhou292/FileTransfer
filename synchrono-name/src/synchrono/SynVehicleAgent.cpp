#include "synchrono/SynVehicleAgent.h"

namespace synchrono {
namespace agent {

SynVehicleAgent::SynVehicleAgent(unsigned int rank, VehicleType type, ChSystem* system)
    : SynAgent(rank, SynAgent::VEHICLE, system), m_vehicle_type(type), m_owns_system(false) {
    m_msg = chrono_types::make_shared<SynVehicleMessage>(rank);

    m_zombie_body = chrono_types::make_shared<ChBodyAuxRef>();
}

void SynVehicleAgent::Advance(double time_of_next_sync) {
    while (m_system->GetChTime() < time_of_next_sync) {
        double time = m_system->GetChTime();

        ChDriver::Inputs driver_inputs = std::static_pointer_cast<SynVehicleBrain>(m_brain)->GetDriverInputs();

        m_brain->Synchronize(time);
        m_terrain->Synchronize(time);
        GetWheeledVehicle().Synchronize(time, driver_inputs, *m_terrain);

        // Advance simulation for one timestep for all modules
        m_brain->Advance(m_step_size);
        m_terrain->Advance(m_step_size);
        GetVehicle().Advance(m_step_size);

        m_vis_manager->Update(m_step_size);

        if (!m_owns_system)
            m_system->DoStepDynamics(m_step_size);
    }
}

void SynVehicleAgent::InitializeZombie(ChSystem* system) {
    if (m_system && system) {
        if (m_verbose)
            std::cerr
                << "SynVehicleAgent::InitializeZombie: Agent has a ChSystem and a ChSystem was passed. Using agent's "
                   "ChSystem."
                << std::endl;
    } else if (!m_system && system) {
        m_system = system;
    } else if (!m_system && !system) {
        std::cerr << "SynVehicleAgent::InitializeZombie: Agent does not have a ChSystem and no ChSystem was passed. A "
                     "segfault would occur. Exiting..."
                  << std::endl;
        exit(1);
    }

    auto trimesh = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();
    trimesh->LoadWavefrontMesh(vehicle::GetDataFile(m_chassis_vis_file), false, false);
    auto trimesh_shape = chrono_types::make_shared<ChTriangleMeshShape>();
    trimesh_shape->SetMesh(trimesh);
    trimesh_shape->SetName(m_chassis_vis_file);
    trimesh_shape->SetStatic(true);

    m_zombie_body->AddAsset(trimesh_shape);
    m_zombie_body->SetCollide(false);
    m_zombie_body->SetBodyFixed(true);
    m_system->Add(m_zombie_body);
    m_zombie_body->SetFrame_COG_to_REF(ChFrame<>({0, 0, -0.2}, {1, 0, 0, 0}));

    auto tire_mesh = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();
    tire_mesh->LoadWavefrontMesh(vehicle::GetDataFile(m_tire_vis_file), false, false);
    auto m_trimesh_tire = chrono_types::make_shared<ChTriangleMeshShape>();
    m_trimesh_tire->SetMesh(tire_mesh);
    m_trimesh_tire->SetStatic(true);
    m_trimesh_tire->SetName(m_tire_vis_file);

    auto wheel_mesh = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();
    wheel_mesh->LoadWavefrontMesh(vehicle::GetDataFile(m_wheel_vis_file), false, false);
    auto m_trimesh_shape_w = chrono_types::make_shared<ChTriangleMeshShape>();
    m_trimesh_shape_w->SetMesh(wheel_mesh);
    m_trimesh_shape_w->SetStatic(true);
    m_trimesh_shape_w->SetName(m_wheel_vis_file);

    for (int i = 0; i < m_wheel_list.size(); i++) {
        m_wheel_list[i] = chrono_types::make_shared<ChBodyAuxRef>();
        m_wheel_list[i]->AddAsset(m_trimesh_shape_w);
        m_wheel_list[i]->AddAsset(m_trimesh_tire);
        m_wheel_list[i]->SetCollide(false);
        m_wheel_list[i]->SetBodyFixed(true);
        m_system->Add(m_wheel_list[i]);
    }
}

void SynVehicleAgent::SynchronizeZombie(SynMessage* message) {
    if (message->GetType() != SynMessage::VEH)
        return;
    auto state = std::static_pointer_cast<SynVehicleMessage::State>(message->GetState());

    m_zombie_body->SetFrame_REF_to_abs(ChFrame<>(state->chassis.pos, state->chassis.rot));
    for (int i = 0; i < state->wheels.size(); i++)
        m_wheel_list[i]->SetFrame_REF_to_abs(ChFrame<>(state->wheels[i].pos, state->wheels[i].rot));
}

std::shared_ptr<SynMessage::State> SynVehicleAgent::GetState() {
    auto chassis = SynMessage::Pose(GetVehicle().GetChassisBody()->GetFrame_REF_to_abs().GetPos(),
                                    GetVehicle().GetChassisBody()->GetRot());

    std::vector<SynMessage::Pose> wheels;
    for (auto axle : GetWheeledVehicle().GetAxles()) {
        for (auto wheel : axle->GetWheels()) {
            auto wheel_state = wheel->GetState();
            wheels.push_back(SynMessage::Pose(wheel_state.pos, wheel_state.rot));
        }
    }

    auto state = chrono_types::make_shared<SynVehicleMessage::State>(m_system->GetChTime(), chassis, wheels);
    m_msg = chrono_types::make_shared<SynVehicleMessage>(m_rank, state);
    return m_msg->GetState();
}

void SynVehicleAgent::ProcessMessage(SynMessage* msg) {
    switch (msg->GetType()) {
        case SynMessage::TERRAIN:
            m_terrain->SyncTerrain(msg);
            break;
        default:
            m_brain->ProcessMessage(msg);
            break;
    }
}

void SynVehicleAgent::GenerateMessagesToSend(std::vector<SynMessage*>& messages) {
    m_terrain->GenerateMessagesToSend(messages);
    m_brain->GenerateMessagesToSend(messages);
    messages.push_back(new SynVehicleMessage(m_rank, GetState()));
}

}  // namespace agent
}  // namespace synchrono
