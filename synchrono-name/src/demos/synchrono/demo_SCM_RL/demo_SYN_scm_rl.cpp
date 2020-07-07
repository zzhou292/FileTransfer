// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Author: Radu Serban
// =============================================================================
//
// Chrono::Vehicle + ChronoParallel demo program for simulating a HMMWV vehicle
// over rigid or granular material.
//
// Contact uses the SMC (penalty) formulation.
//
// The global reference frame has Z up.
// All units SI.
// =============================================================================

#include "InferenceDriver.h"
#include "SynInferenceBrain.h"

#include "synchrono/SynCLI.h"
#include "synchrono/SynVisManager.h"

#include "synchrono/visualization/SynSenVehicleVis.h"
#include "synchrono/visualization/SynIrrVehicleVis.h"

#include "chrono/utils/ChUtilsInputOutput.h"

#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/ChDriver.h"
#include "chrono_vehicle/terrain/SCMDeformableTerrain.h"
#include "chrono_vehicle/terrain/RigidTerrain.h"

#include "chrono_models/vehicle/hmmwv/HMMWV.h"

#include "chrono_thirdparty/filesystem/path.h"

#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterSave.h"
#include "chrono_sensor/utils/ChVisualMaterialUtils.h"
#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono/physics/ChBodyEasy.h"
#include <chrono>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::visualization;

using namespace chrono;
using namespace chrono::collision;
using namespace chrono::vehicle;
using namespace chrono::vehicle::hmmwv;
using namespace chrono::sensor;

// =============================================================================
// USER SETTINGS
// =============================================================================

// -----------------------------------------------------------------------------
// Vehicle parameters
// -----------------------------------------------------------------------------

// Type of wheel/tire (controls both contact and visualization)
enum WheelType { CYLINDRICAL, LUGGED };

// Type of terrain
enum TerrainType { RIGID_SOIL, DEFORMABLE_CUSTOM, DEFORMABLE_SOFT, DEFORMABLE_HARD };

// Type of powertrain model (SHAFTS, SIMPLE)
PowertrainModelType powertrain_model = PowertrainModelType::SHAFTS;

// Drive type (FWD, RWD, or AWD)
DrivelineType drive_type = DrivelineType::AWD;

// Chassis visualization (MESH, PRIMITIVES, NONE)
VisualizationType chassis_vis = VisualizationType::PRIMITIVES;

// -----------------------------------------------------------------------------
// Simulation parameters
// -----------------------------------------------------------------------------

// Time interval between two render frames (1/FPS)
double render_step_size = 1.0 / 100;

// Point on chassis tracked by the camera
ChVector<> trackPoint(0.0, 0.0, 1.75);

// Output directories
const std::string out_dir = GetChronoOutputPath() + "HMMWV_DEF_SOIL";
const std::string img_dir = out_dir + "/IMG";

// Visualization output
bool img_output = false;

// =============================================================================

void CreateLuggedGeometry(std::shared_ptr<ChBody> wheelBody) {
    std::string lugged_file("hmmwv/lugged_wheel_section.obj");
    geometry::ChTriangleMeshConnected lugged_mesh;
    ChConvexDecompositionHACDv2 lugged_convex;
    utils::LoadConvexMesh(vehicle::GetDataFile(lugged_file), lugged_mesh, lugged_convex);
    int num_hulls = lugged_convex.GetHullCount();

    auto coll_model = wheelBody->GetCollisionModel();
    coll_model->ClearModel();

    // Add a cylinder to represent the wheel hub.
    MaterialInfo minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto mat = minfo.CreateMaterial(ChContactMethod::NSC);

    // Assemble the tire contact from 15 segments, properly offset.
    // Each segment is further decomposed in convex hulls.
    for (int iseg = 0; iseg < 15; iseg++) {
        ChQuaternion<> rot = Q_from_AngAxis(iseg * 24 * CH_C_DEG_TO_RAD, VECT_Y);
        for (int ihull = 0; ihull < num_hulls; ihull++) {
            std::vector<ChVector<> > convexhull;
            lugged_convex.GetConvexHullResult(ihull, convexhull);
            coll_model->AddConvexHull(mat, convexhull, VNULL, rot);
        }
    }

    coll_model->AddCylinder(mat, 0.223, 0.223, 0.126);
    coll_model->BuildModel();

    // Visualization
    auto trimesh = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();
    trimesh->LoadWavefrontMesh(vehicle::GetDataFile("hmmwv/lugged_wheel.obj"), false, false);

    auto trimesh_shape = chrono_types::make_shared<ChTriangleMeshShape>();
    trimesh_shape->SetMesh(trimesh);
    trimesh_shape->SetName("lugged_wheel");
    trimesh_shape->SetStatic(true);
    wheelBody->AddAsset(trimesh_shape);

    auto mcolor = chrono_types::make_shared<ChColorAsset>(0.3f, 0.3f, 0.3f);
    wheelBody->AddAsset(mcolor);
}

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();

    // Demo specific parameters
    cli.AddOption("Demo", cxxopts::Option("sim_out_dir", "Output directory for simulation logging",
                                          cxxopts::value<std::string>()->default_value("SCM_RL_OUTPUT/")));
    cli.AddOption("Demo", cxxopts::Option("record", "Log simulation parameters and data",
                                          cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo", cxxopts::Option("x,size_x", "Size in the x", cxxopts::value<double>()->default_value("100")));
    cli.AddOption("Demo", cxxopts::Option("y,size_y", "Size in the y", cxxopts::value<double>()->default_value("100")));
    // Would rather have it be w and h, but h is taken by the --help flag
    cli.AddOption("Demo",
                  cxxopts::Option("width", "Camera resolution width", cxxopts::value<double>()->default_value("1920")));
    cli.AddOption(
        "Demo", cxxopts::Option("height", "Camera resolution height", cxxopts::value<double>()->default_value("1080")));
    cli.AddOption(
        "Demo",
        cxxopts::Option("terrain",
                        "Terrain type: 0 = RIGID_SOIL, 1 = DEFORMABLE_CUSTOM, 2 = DEFORMABLE_SOFT, 3 = DEFORMABLE_HARD",
                        cxxopts::value<int>()->default_value("0")));
    cli.AddOption("Demo", cxxopts::Option("wheel", "Wheel type: 0 = CYLINDRICAL, 1 = LUGGED",
                                          cxxopts::value<int>()->default_value("0")));

    // SCM parameters
    cli.AddOption("SCM", cxxopts::Option("d,dpu", "Divisions per unit", cxxopts::value<double>()->default_value("20")));
    cli.AddOption("SCM", cxxopts::Option("Y_t", "Y_t", cxxopts::value<double>()->default_value("1.0e6")));
    cli.AddOption("SCM", cxxopts::Option("cr_t", "cr_t", cxxopts::value<double>()->default_value("0.1")));
    cli.AddOption("SCM", cxxopts::Option("mu_t", "mu_t", cxxopts::value<double>()->default_value("0.8")));
    cli.AddOption("SCM", cxxopts::Option("Kphi", "Kphi, frictional modulus in Bekker model",
                                         cxxopts::value<double>()->default_value("2e6")));
    cli.AddOption("SCM", cxxopts::Option("Kc", "Kc, cohesive modulus in Bekker model",
                                         cxxopts::value<double>()->default_value("0")));
    cli.AddOption("SCM", cxxopts::Option("Bn", "n, exponent of sinkage in Bekker model (usually 0.6...1.8)",
                                         cxxopts::value<double>()->default_value("1.1")));
    cli.AddOption("SCM", cxxopts::Option("cohesion", "Cohesion in, Pa, for shear failure",
                                         cxxopts::value<double>()->default_value("0")));
    cli.AddOption("SCM", cxxopts::Option("friction", "Friction angle (in degrees!), for shear failure",
                                         cxxopts::value<double>()->default_value("30")));
    cli.AddOption(
        "SCM",
        cxxopts::Option("shear", "J , shear parameter, in meters, in Janosi-Hanamoto formula (usually few mm or cm)",
                        cxxopts::value<double>()->default_value("0.01")));
    cli.AddOption("SCM",
                  cxxopts::Option("elastic",
                                  "elastic stiffness K (must be > Kphi; very high values gives the original SCM model)",
                                  cxxopts::value<double>()->default_value("40e7")));
    cli.AddOption(
        "SCM", cxxopts::Option(
                   "damping",
                   "vertical damping R, per unit area (vertical speed proportional, it is zero in original SCM model)",
                   cxxopts::value<double>()->default_value("3e4")));

    cli.Parse(argc, argv);
    cli.SetVerbosity(true);

    // Set values
    // Contact material properties
    const double Y_t = cli.GetAsType<double>("Y_t");
    const double cr_t = cli.GetAsType<double>("cr_t");
    const double mu_t = cli.GetAsType<double>("mu_t");
    const WheelType wheel_type = (WheelType)cli.GetAsType<int>("wheel");
    const TerrainType terrain_type = (TerrainType)cli.GetAsType<int>("terrain");
    const double size_x = cli.GetAsType<double>("size_x");
    const double size_y = cli.GetAsType<double>("size_y");
    const double divs_per_units = cli.GetAsType<double>("dpu");
    const double CAM_RES_WIDTH = cli.GetAsType<double>("width");
    const double CAM_RES_HEIGHT = cli.GetAsType<double>("height");
    const double step_size = cli.GetAsType<double>("step_size");
    const double time_end = cli.GetAsType<double>("time_end");
    const std::string out_dir = cli.GetAsType<std::string>("sim_out_dir");
    const bool RECORD = cli.GetAsType<bool>("record");

    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    // --------------------
    // Create HMMWV vehicle
    // --------------------
    // Initial vehicle position and orientation
    ChVector<> initLoc(-size_x / 2.25, -size_y / 2.25, 0.5);
    ChQuaternion<> initRot(1, 0, 0, 0);

    HMMWV_Reduced my_hmmwv;
    my_hmmwv.SetContactMethod(ChContactMethod::SMC);
    my_hmmwv.SetChassisFixed(false);
    my_hmmwv.SetInitPosition(ChCoordsys<>(initLoc, initRot));
    // my_hmmwv.SetPowertrainType(powertrain_model);
    // my_hmmwv.SetDriveType(drive_type);
    my_hmmwv.SetTireType(TireModelType::RIGID);
    my_hmmwv.SetTireStepSize(step_size);
    my_hmmwv.Initialize();

    my_hmmwv.SetChassisVisualizationType(VisualizationType::MESH);
    my_hmmwv.SetSuspensionVisualizationType(VisualizationType::NONE);
    my_hmmwv.SetSteeringVisualizationType(VisualizationType::NONE);
    my_hmmwv.SetWheelVisualizationType(VisualizationType::MESH);
    my_hmmwv.SetTireVisualizationType(VisualizationType::MESH);

    ChSystem* system = my_hmmwv.GetSystem();

    // --------------------------------------------------------
    // Set wheel contact material.
    // If needed, modify wheel contact and visualization models
    // --------------------------------------------------------
    // for (auto& axle : my_hmmwv.GetVehicle().GetAxles()) {
    //     auto wheelBodyL = axle->m_wheels[0]->GetSpindle();
    //     wheelBodyL->GetMaterialSurfaceSMC()->SetFriction(mu_t);
    //     wheelBodyL->GetMaterialSurfaceSMC()->SetYoungModulus(Y_t);
    //     wheelBodyL->GetMaterialSurfaceSMC()->SetRestitution(cr_t);
    //     // CreateLuggedGeometry(wheelBodyL);
    //     auto wheelBodyR = axle->m_wheels[1]->GetSpindle();
    //     wheelBodyR->GetMaterialSurfaceSMC()->SetFriction(mu_t);
    //     wheelBodyR->GetMaterialSurfaceSMC()->SetYoungModulus(Y_t);
    //     wheelBodyR->GetMaterialSurfaceSMC()->SetRestitution(cr_t);
    //     // CreateLuggedGeometry(wheelBodyR);
    // }

    // ------------------
    // Create the terrain
    // ------------------
    ChTerrain* terrain;

    MaterialInfo minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto patch_mat = minfo.CreateMaterial(ChContactMethod::NSC);

    auto CreateDeformableTerrain = [&](SCMDeformableTerrain* terrainD) {
        /*
        terrainD->SetBulldozingFlow(true);    // inflate soil at the border of the rut
        terrainD->SetBulldozingParameters(55, // angle of friction for erosion of displaced material at the border
        of the rut 0.8, // displaced material vs downward pressed material. 5,   // number of erosion refinements
        per timestep 10); // number of concentric vertex selections subject to erosion
        */
        // Turn on the automatic level of detail refinement, so a coarse terrain mesh
        // is automatically improved by adding more points under the wheel contact patch:
        terrainD->SetAutomaticRefinement(true);
        terrainD->SetAutomaticRefinementResolution(0.04);

        ////terrainD->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 80, 16);
        ////terrainD->SetPlotType(vehicle::SCMDeformableTerrain::PLOT_PRESSURE_YELD, 0, 30000.2);
        terrainD->SetPlotType(vehicle::SCMDeformableTerrain::PLOT_SINKAGE, 0, 0.1);

        terrainD->Initialize(0, size_x, size_y, divs_per_units * size_x, divs_per_units * size_y);

        terrainD->AddMovingPatch(my_hmmwv.GetChassisBody(), ChVector<>(0, 0, 0), 5, 5);

        terrain = terrainD;

        auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
        vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
        vis_mat->SetFresnelMax(0.2);
        vis_mat->SetSpecularColor({0.2, 0.2, 0.2});
        terrainD->GetMesh()->material_list.push_back(vis_mat);
    };

    std::cout << "Creating Terrain of Type: ";
    switch (terrain_type) {
        case DEFORMABLE_CUSTOM: {
            std::cout << "DEFORMABLE_CUSTOM" << std::endl;
            SCMDeformableTerrain* terrainD = new SCMDeformableTerrain(system);
            terrainD->SetSoilParameters(
                cli.GetAsType<double>("Kphi"),      // Bekker Kphi
                cli.GetAsType<double>("Kc"),        // Bekker Kc
                cli.GetAsType<double>("Bn"),        // Bekker n exponent
                cli.GetAsType<double>("cohesion"),  // Mohr cohesive limit (Pa)
                cli.GetAsType<double>("friction"),  // Mohr friction limit (degrees)
                cli.GetAsType<double>("shear"),     // Janosi shear coefficient (m)
                cli.GetAsType<double>("elastic"),   // Elastic stiffness (Pa/m), before plastic yield
                cli.GetAsType<double>(
                    "damping")  // Damping (Pa s/m), proportional to negative vertical speed (optional)
            );
            CreateDeformableTerrain(terrainD);
            break;
        }
        case DEFORMABLE_SOFT: {
            std::cout << "DEFORMABLE_SOFT" << std::endl;
            SCMDeformableTerrain* terrainD = new SCMDeformableTerrain(system);
            terrainD->SetSoilParameters(0.2e6, 0, 1.1, 0, 30, 0.01, 4e7, 3e4);
            CreateDeformableTerrain(terrainD);
            break;
        }
        case DEFORMABLE_HARD: {
            std::cout << "DEFORMABLE_HARD" << std::endl;
            SCMDeformableTerrain* terrainD = new SCMDeformableTerrain(system);
            terrainD->SetSoilParameters(5301e3, 102e3, 0.793, 1.3e3, 31.1, 2e-2, 4e8, 3e4);
            CreateDeformableTerrain(terrainD);
            break;
        }

        case RIGID_SOIL: {
            std::cout << "RIGID_SOIL" << std::endl;
            RigidTerrain* terrainR = new RigidTerrain(system);
            auto patch = terrainR->AddPatch(patch_mat, ChVector<>(0, 0, 0), ChVector<>(0, 0, 1), size_x, size_y);
            patch->SetColor(ChColor(0.8f, 0.8f, 0.5f));
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/tile4.jpg"), 200, 200);
            terrainR->Initialize();

            terrain = terrainR;

            auto patch_asset = patch->GetGroundBody()->GetAssets()[0];
            if (std::shared_ptr<ChVisualization> visual_asset =
                    std::dynamic_pointer_cast<ChVisualization>(patch_asset)) {
                std::shared_ptr<ChVisualMaterial> box_texture = chrono_types::make_shared<ChVisualMaterial>();
                box_texture->SetKdTexture(chrono::GetChronoDataFile("sensor/textures/grass_texture.jpg"));
                box_texture->SetFresnelMax(0.2);
                box_texture->SetSpecularColor({0.2, 0.2, 0.2});
                visual_asset->material_list.push_back(box_texture);
            }

            break;
        }
    }

    std::cout << "Done Creating Terrain" << std::endl;

    // Add goal sphere
    auto goal = ChVector<>(size_x / 2.25, size_y / 2.25, 1);
    auto goal_sphere = chrono_types::make_shared<ChBodyEasySphere>(.25, 1000, false, true);
    goal_sphere->SetBodyFixed(true);
    goal_sphere->SetPos(goal);
    auto asset = goal_sphere->GetAssets()[0];
    if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(asset)) {
        auto color = chrono_types::make_shared<ChVisualMaterial>();
        color->SetDiffuseColor({1, 0, 0});
        color->SetSpecularColor({1, 0, 0});
        color->SetFresnelMax(1);
        color->SetFresnelMin(0);
        visual_asset->material_list.push_back(color);
    }
    system->Add(goal_sphere);

    // Create sensors to be used during inference
    // First create the sensor manager and setup the environment
    auto vis_manager = chrono_types::make_shared<SynVisManager>();
    auto camera_vis = chrono_types::make_shared<SynSenVehicleVis>();
    camera_vis->InitializeDefaultSensorManager(system);
    camera_vis->GetSensorManager()->scene->GetBackground().has_texture = true;
    camera_vis->GetSensorManager()->scene->GetBackground().env_tex = "sensor/textures/qwantani_8k.hdr";
    camera_vis->GetSensorManager()->scene->GetBackground().has_changed = true;

    // Camera attached to the front of agent
    int camera_update_rate = 30;  // Camera update rate [Hz]
    int camera_width = 80;        // Camera width [pixels]
    int camera_height = 45;       // Camera height [pixels]
    auto camera =
        chrono_types::make_shared<ChCameraSensor>(my_hmmwv.GetVehicle().GetChassisBody(),  // body camera is attached to
                                                  camera_update_rate,                      // update rate in Hz
                                                  ChFrame<>({1.5, 0, .875}, {1, 0, 0, 0}),  // offset pose
                                                  camera_width,                             // image width
                                                  camera_height,                            // image height
                                                  CH_C_PI / 3.,                             // horizontal field of view
                                                  1. / camera_update_rate,                  // lag
                                                  0);                                       // exposure time
    camera->SetName("Camera Sensor");
    camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());
    camera_vis->SetSensor(camera);
    vis_manager->AttachVis(camera_vis);

    if (cli.HasValueInVector<int>("sens", 1)) {
        auto third_person_camera_vis = chrono_types::make_shared<SynSenVehicleVis>();
        third_person_camera_vis->SetSensorManager(camera_vis->GetSensorManager());
        third_person_camera_vis->InitializeAsDefaultChaseCamera(my_hmmwv.GetVehicle().GetChassisBody());
        if (cli.GetAsType<bool>("sens_save")) {
            std::string path = out_dir + "SENSOR_OUTPUT/RL_third_person/";
            third_person_camera_vis->AddFilterSave(path);
        }
        vis_manager->AttachVis(third_person_camera_vis);

        auto birds_eye_camera_vis = chrono_types::make_shared<SynSenVehicleVis>();
        birds_eye_camera_vis->SetSensorManager(camera_vis->GetSensorManager());
        auto origin = chrono_types::make_shared<ChBodyAuxRef>();
        origin->SetBodyFixed(true);
        system->AddBody(origin);
        birds_eye_camera_vis->InitializeAsDefaultBirdsEyeCamera(origin, 150);
        if (cli.GetAsType<bool>("sens_save")) {
            std::string path = out_dir + "SENSOR_OUTPUT/RL_birds_eye/";
            birds_eye_camera_vis->AddFilterSave(path);
        }
        vis_manager->AttachVis(birds_eye_camera_vis);
    }

    // Create inference driver
    auto driver = chrono_types::make_shared<InferenceDriver>(
        my_hmmwv.GetVehicle(), GetChronoDataFile("sensor/neural_nets/rl_SCM_simple.onnx"), camera);
    driver->Initialize();
    driver->SetGoalPosition(goal);

    // Initialize output
    if (!filesystem::create_directory(filesystem::path(out_dir))) {
        std::cout << "Error creating directory " << out_dir << std::endl;
        return 1;
    }
    std::string log_file = out_dir + "/rl_sim_log.txt";
    utils::CSV_writer log_csv(" ");

    std::string param_file = out_dir + "/rl_sim_param.txt";
    utils::CSV_writer param_csv(" ");
    if (RECORD) {
        // param_csv << ;
        param_csv.write_to_file(param_file);
    }

    // Create irrlicht vis
    if (cli.HasValueInVector<int>("irr", 1)) {
        auto irr_vis = chrono_types::make_shared<SynIrrVehicleVis>(driver, step_size);
        irr_vis->InitializeAsDefaultChaseCamera(my_hmmwv.GetVehicle());
        irr_vis->SetSave(cli.GetAsType<bool>("irr_save"));
        irr_vis->SetVisualize(false);
        vis_manager->AttachVis(irr_vis);
    }

    // ---------------
    // Simulation loop
    // ---------------

    // Solver settings.
    system->SetSolverMaxIterations(50);

    // Number of simulation steps between two 3D view render frames
    int render_steps = (int)std::ceil(render_step_size / step_size);

    // Initialize simulation frame counter
    int step_number = 0;
    int render_frame = 0;
    int last_update = -1;
    double control_frequency = 10;
    int updates = 1;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    while (true) {
        double time = system->GetChTime();
        if (time > time_end) {
            std::cout << "Time out!" << std::endl;
            break;
        }

        // Driver inputs
        ChDriver::Inputs driver_inputs = driver->GetInputs();

        // Update modules
        // synchronize at prescribed control frequency
        if (time - last_update >= 1 / control_frequency) {
            driver->Synchronize(time);
            last_update = time;

            // Record data, if necessary
            if (RECORD) {
                auto pos = my_hmmwv.GetVehicle().GetChassisBody()->GetPos();
                auto vel = my_hmmwv.GetVehicle().GetChassisBody()->GetPos_dt();
                auto acc = my_hmmwv.GetVehicle().GetChassisBody()->GetPos_dtdt();
                auto err = (pos - goal);
                log_csv << time;
                log_csv << pos.x() << pos.y() << pos.z();
                log_csv << vel.x() << vel.y() << vel.z();
                log_csv << acc.x() << acc.y() << acc.z();
                log_csv << err.x() << err.y() << err.z();
                log_csv << std::endl;
            }
        }
        terrain->Synchronize(time);
        my_hmmwv.Synchronize(time, driver_inputs, *terrain);

        // Advance dynamics
        system->DoStepDynamics(step_size);
        driver->Advance(step_size);

        // Update vis manager
        vis_manager->Update(step_size);

        // Increment frame number
        step_number++;

        if (time - updates > 0) {
            std::chrono::high_resolution_clock::time_point time_now = std::chrono::high_resolution_clock::now();
            auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(time_now - start);
            std::cout << "Sim Time: " << (time) << ", Avg. Real Time: " << ((time_span.count() / 1e6) / time)
                      << std::endl;
            updates++;

            if (RECORD) {
                log_csv.write_to_file(log_file);
            }
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Total time: " << (time_span.count() / 1e6) / time_end << std::endl;

    if (RECORD) {
        log_csv.write_to_file(log_file);
    }

    // Cleanup
    delete terrain;

    std::cout << "Ended successfully." << std::endl;

    return 0;
}
