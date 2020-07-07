#include "synchrono/agent/SynFullHMMWVAgent.h"
#include "synchrono/agent/SynHMMWVAgent.h"

namespace synchrono {
namespace agent {

SynFullHMMWVAgent::SynFullHMMWVAgent(int rank, ChSystem* system)
    : SynVehicleAgent(rank, SynVehicleAgent::HMMWV, system) {
    if (system) {
        m_hmmwv = chrono_types::make_shared<HMMWV_Full>(system);
        m_system = system;
    } else {
        m_hmmwv = chrono_types::make_shared<HMMWV_Full>();
        m_owns_system = true;
    }

    m_wheel_list.resize(4);
    m_use_lugged_tires = false;

    // Set visualization files
    m_chassis_vis_file = "hmmwv/hmmwv_chassis.obj";
    m_wheel_vis_file = "hmmwv/hmmwv_rim.obj";
    m_tire_vis_file = "hmmwv/hmmwv_tire_left.obj";
}

void SynFullHMMWVAgent::InitializeVehicle(const ChCoordsys<>& pose) {
    // Setup the HMMWV object that will represent this hmmwv
    m_hmmwv->SetContactMethod(ChContactMethod::NSC);
    m_hmmwv->SetChassisCollisionType(ChassisCollisionType::NONE);
    m_hmmwv->SetChassisFixed(false);
    m_hmmwv->SetPowertrainType(PowertrainModelType::SHAFTS);
    m_hmmwv->SetDriveType(DrivelineType::AWD);
    m_hmmwv->SetInitPosition(pose);
    m_hmmwv->SetTireType(TireModelType::RIGID);
    m_hmmwv->SetTireStepSize(m_step_size);
    m_hmmwv->Initialize();

    m_hmmwv->SetSuspensionVisualizationType(VisualizationType::MESH);
    m_hmmwv->SetSteeringVisualizationType(VisualizationType::NONE);

    if (m_use_lugged_tires) {
        m_hmmwv->SetWheelVisualizationType(VisualizationType::NONE);
        m_hmmwv->SetChassisVisualizationType(VisualizationType::NONE);
    } else {
        m_hmmwv->SetWheelVisualizationType(VisualizationType::MESH);
        m_hmmwv->SetChassisVisualizationType(VisualizationType::MESH);

        m_hmmwv->SetTireVisualizationType(VisualizationType::MESH);
    }

    // auto patch_asset = m_hmmwv->GetChassis()->GetBody()->GetAssets()[0];
    // if (auto visual_asset = std::dynamic_pointer_cast<ChVisualization>(patch_asset)) {
    //     auto tire_texture = chrono_types::make_shared<ChVisualMaterial>();
    //     tire_texture->SetFresnelMax(0.2);
    //     tire_texture->SetSpecularColor({1, 0, 0});
    // }

    if (m_owns_system)
        m_system = m_hmmwv->GetSystem();

    if (m_use_lugged_tires) {
        // Contact material properties
        float mu_t = 0.8f;
        float Y_t = 1.0e6f;
        float cr_t = 0.1f;

        auto wheel_material = chrono_types::make_shared<ChMaterialSurfaceSMC>();
        wheel_material->SetFriction(mu_t);
        wheel_material->SetYoungModulus(Y_t);
        wheel_material->SetRestitution(cr_t);

        for (auto& axle : GetHMMWV()->GetVehicle().GetAxles()) {
            auto wheelBodyL = axle->m_wheels[0]->GetSpindle();
            SynHMMWVAgent::CreateLuggedGeometry(wheelBodyL, wheel_material);

            auto wheelBodyR = axle->m_wheels[1]->GetSpindle();
            SynHMMWVAgent::CreateLuggedGeometry(wheelBodyR, wheel_material);
        }

        // m_tire_vis_file = "hmmwv/lugged_wheel.obj";
    }
}

void SynFullHMMWVAgent::SetZombieTexture() {
#ifdef SENSOR
    for (auto wheel : m_wheel_list) {
        auto patch_asset = wheel->GetAssets()[0];
        if (auto visual_asset = std::dynamic_pointer_cast<ChVisualization>(patch_asset)) {
            auto tire_texture = chrono_types::make_shared<ChVisualMaterial>();

            tire_texture->SetDiffuseColor({.2f, .2f, .2f});
            tire_texture->SetSpecularColor({.1f, .1f, .1f});
            tire_texture->SetRoughness(.5);
            tire_texture->SetFresnelMax(.1);
            visual_asset->material_list.push_back(tire_texture);
        }
    }
#endif  // SENSOR
}

}  // namespace agent
}  // namespace synchrono
