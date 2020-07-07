#include "synchrono/terrain/SynRigidTerrain.h"

namespace synchrono {
namespace agent {

SynRigidTerrain::SynRigidTerrain(ChSystem* system, std::string collision_mesh_path, std::string vis_mesh_path) {
    // Mesh

    MaterialInfo minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto patch_mat = minfo.CreateMaterial(ChContactMethod::SMC);

    auto terrain = chrono_types::make_shared<RigidTerrain>(system);
    std::shared_ptr<RigidTerrain::Patch> patch;
    patch = terrain->AddPatch(patch_mat, CSYSNORM, collision_mesh_path, "mesh", 0.01, false);

    auto vis_mesh = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();
    vis_mesh->LoadWavefrontMesh(vis_mesh_path, true, true);

    auto trimesh_shape = chrono_types::make_shared<ChTriangleMeshShape>();
    trimesh_shape->SetMesh(vis_mesh);
    trimesh_shape->SetName("mesh_name");
    trimesh_shape->SetStatic(true);
    patch->GetGroundBody()->AddAsset(trimesh_shape);

    Initialize(patch, terrain);
}

SynRigidTerrain::SynRigidTerrain(ChSystem* system, double terrainHeight, double terrainLength, double terrainWidth) {
    // Box

    MaterialInfo minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto patch_mat = minfo.CreateMaterial(ChContactMethod::NSC);

    auto terrain = chrono_types::make_shared<RigidTerrain>(system);
    std::shared_ptr<RigidTerrain::Patch> patch;

    // For Irrlicht

    patch = terrain->AddPatch(patch_mat, ChVector<>(0, 0, 0), ChVector<>(0, 0, 1), terrainLength, terrainWidth);
    patch->SetTexture(vehicle::GetDataFile("terrain/textures/tile4.jpg"), 200, 200);

#ifdef SENSOR
    // For OptiX
    // Note: might not always be the 0th texture, may need to loop until you find one that
    //          casts correctly
    auto patch_asset = patch->GetGroundBody()->GetAssets()[0];
    if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(patch_asset)) {
        std::shared_ptr<ChVisualMaterial> box_texture = chrono_types::make_shared<ChVisualMaterial>();
        // box_texture->SetKdTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"));
        box_texture->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
        // box_texture->SetKdTexture(GetChronoDataFile("sensor/textures/snow.jpg"));
        // FresnelMax and SpecularColor should make it less shiny
        box_texture->SetFresnelMax(0.2);
        box_texture->SetSpecularColor({0.2, 0.2, 0.2});

        visual_asset->material_list.push_back(box_texture);
    }
#endif  // SENSOR

    Initialize(patch, terrain);
}

SynRigidTerrain::SynRigidTerrain(ChSystem* system) {
    // Height Map

    MaterialInfo minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto patch_mat = minfo.CreateMaterial(ChContactMethod::SMC);

    auto terrain = chrono_types::make_shared<RigidTerrain>(system);
    std::shared_ptr<RigidTerrain::Patch> patch;
    patch = terrain->AddPatch(patch_mat, CSYSNORM, vehicle::GetDataFile("terrain/height_maps/test64.bmp"), "test64",
                              128, 128, 0, 4);
    patch->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 16, 16);

    Initialize(patch, terrain);
}

// Jay - Use of ifdef here is only for compatibility with Chrono 5.0.1, not really related to SCM terrain...
#ifdef SYN_SCM
SynRigidTerrain::SynRigidTerrain(ChSystem* system,
                                 double terrainLength,
                                 double terrainWidth,
                                 std::shared_ptr<ChBezierCurve> path,
                                 double height,
                                 int width,
                                 double dx,
                                 double dy) {
    MaterialInfo minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto patch_mat = minfo.CreateMaterial(ChContactMethod::NSC);

    MaterialInfo minfo1;
    minfo1.mu = 0.9f;
    minfo1.cr = 0.01f;
    minfo1.Y = 2e7f;
    auto patch_mat1 = minfo1.CreateMaterial(ChContactMethod::NSC);

    auto terrain = chrono_types::make_shared<RigidTerrain>(system);
    std::shared_ptr<RigidTerrain::Patch> patch, path_patch;

    path_patch = terrain->AddPatch(patch_mat1, ChCoordsys<>({0, 0, 0}, QUNIT), path, "Path Mesh", height, width, dx, dy,
                                   0, false);
    patch = terrain->AddPatch(patch_mat, ChVector<>(0, 0, 0), ChVector<>(0, 0, 1), terrainLength * 2, terrainWidth * 2);
    patch->GetGroundBody()->SetCollide(false);
    patch->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 16, 16);
#ifdef SENSOR
    // For OptiX
    // Note: might not always be the 0th texture, may need to loop until you find one that
    //          casts correctly
    auto patch_asset = patch->GetGroundBody()->GetAssets()[0];
    if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(patch_asset)) {
        std::shared_ptr<ChVisualMaterial> box_texture = chrono_types::make_shared<ChVisualMaterial>();
        // box_texture->SetKdTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"));
        box_texture->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
        // box_texture->SetKdTexture(GetChronoDataFile("sensor/textures/snow.jpg"));
        // FresnelMax and SpecularColor should make it less shiny
        box_texture->SetFresnelMax(0.2);
        box_texture->SetSpecularColor({0.2, 0.2, 0.2});

        visual_asset->material_list.push_back(box_texture);
    }
#endif  // SENSOR

    Initialize(patch, terrain);
}
#endif  // SCM

void SynRigidTerrain::Initialize(std::shared_ptr<RigidTerrain::Patch> patch, std::shared_ptr<RigidTerrain> terrain) {
    patch->SetColor(ChColor(0.8f, 0.8f, 0.5f));
    terrain->Initialize();

    m_terrain = terrain;
}

ChVector<double> SynRigidTerrain::GetNormal(const ChVector<double>& loc) const {
    return GetRigidTerrain()->GetNormal(loc);
}

double SynRigidTerrain::GetHeight(const ChVector<double>& loc) const {
    return GetRigidTerrain()->GetHeight(loc);
}

float SynRigidTerrain::GetCoefficientFriction(const ChVector<double>& loc) const {
    return GetRigidTerrain()->GetCoefficientFriction(loc);
}

}  // namespace agent
}  // namespace synchrono
