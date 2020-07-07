#include "synchrono/agent/SynHMMWVAgent.h"

namespace synchrono {
namespace agent {

SynHMMWVAgent::SynHMMWVAgent(int rank, ChSystem* system)
    : SynVehicleAgent(rank, SynVehicleAgent::HMMWV, system), m_use_lugged_tires(false) {
    if (system) {
        m_hmmwv = chrono_types::make_shared<HMMWV_Reduced>(system);
        m_system = system;
    } else {
        m_hmmwv = chrono_types::make_shared<HMMWV_Reduced>();
        m_owns_system = true;
    }

    m_wheel_list.resize(4);
    m_use_lugged_tires = false;

    // Set visualization files
    m_chassis_vis_file = "hmmwv/hmmwv_chassis.obj";
    m_wheel_vis_file = "hmmwv/hmmwv_rim.obj";
    m_tire_vis_file = "hmmwv/hmmwv_tire_left.obj";
}

void SynHMMWVAgent::InitializeVehicle(const ChCoordsys<>& pose) {
    // Setup the HMMWV object that will represent this hmmwv
    m_hmmwv->SetContactMethod(ChContactMethod::NSC);
    m_hmmwv->SetChassisCollisionType(ChassisCollisionType::NONE);
    m_hmmwv->SetChassisFixed(false);
    m_hmmwv->SetInitPosition(pose);
    m_hmmwv->SetTireType(TireModelType::RIGID);
    m_hmmwv->SetTireStepSize(m_step_size);
    m_hmmwv->Initialize();

    m_hmmwv->SetChassisVisualizationType(VisualizationType::MESH);
    m_hmmwv->SetSuspensionVisualizationType(VisualizationType::NONE);
    m_hmmwv->SetSteeringVisualizationType(VisualizationType::NONE);
    m_hmmwv->SetWheelVisualizationType(VisualizationType::MESH);
    m_hmmwv->SetTireVisualizationType(VisualizationType::MESH);

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

        for (auto& axle : GetWheeledVehicle().GetAxles()) {
            auto wheelBodyL = axle->m_wheels[0]->GetSpindle();
            CreateLuggedGeometry(wheelBodyL, wheel_material);

            auto wheelBodyR = axle->m_wheels[1]->GetSpindle();
            CreateLuggedGeometry(wheelBodyR, wheel_material);
        }

        m_wheel_vis_file = "hmmwv/lugged_wheel.obj";
    }
}

void SynHMMWVAgent::CreateLuggedGeometry(std::shared_ptr<ChBody> wheel_body,
                                         std::shared_ptr<ChMaterialSurfaceSMC> wheel_material) {
    std::string lugged_file("hmmwv/lugged_wheel_section.obj");
    geometry::ChTriangleMeshConnected lugged_mesh;
    collision::ChConvexDecompositionHACDv2 lugged_convex;
    utils::LoadConvexMesh(vehicle::GetDataFile(lugged_file), lugged_mesh, lugged_convex);
    int num_hulls = lugged_convex.GetHullCount();

    auto coll_model = wheel_body->GetCollisionModel();
    coll_model->ClearModel();

    // Assemble the tire contact from 15 segments, properly offset.
    // Each segment is further decomposed in convex hulls.
    for (int iseg = 0; iseg < 15; iseg++) {
        ChQuaternion<> rot = Q_from_AngAxis(iseg * 24 * CH_C_DEG_TO_RAD, VECT_Y);
        for (int ihull = 0; ihull < num_hulls; ihull++) {
            std::vector<ChVector<> > convexhull;
            lugged_convex.GetConvexHullResult(ihull, convexhull);
            coll_model->AddConvexHull(wheel_material, convexhull, VNULL, rot);
        }
    }

    // Add a cylinder to represent the wheel hub.
    coll_model->AddCylinder(wheel_material, 0.223, 0.223, 0.126);
    coll_model->BuildModel();

    // Visualization
    auto trimesh = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();
    trimesh->LoadWavefrontMesh(vehicle::GetDataFile("hmmwv/lugged_wheel.obj"), false, false);

    auto trimesh_shape = chrono_types::make_shared<ChTriangleMeshShape>();
    trimesh_shape->SetMesh(trimesh);
    trimesh_shape->SetName("lugged_wheel");
    trimesh_shape->SetStatic(true);
    wheel_body->AddAsset(trimesh_shape);

    auto mcolor = chrono_types::make_shared<ChColorAsset>(0.3f, 0.3f, 0.3f);
    wheel_body->AddAsset(mcolor);

#ifdef SENSOR
    auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
    vis_mat->SetDiffuseColor({.2f, .2f, .2f});
    vis_mat->SetSpecularColor({.1f, .1f, .1f});
    vis_mat->SetRoughness(.5);
    vis_mat->SetFresnelMax(.1);
    trimesh_shape->material_list.push_back(vis_mat);
#endif  // SENSOR
}

void SynHMMWVAgent::SetZombieTexture() {
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
