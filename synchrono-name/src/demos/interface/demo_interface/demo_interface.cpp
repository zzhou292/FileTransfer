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
// Authors: Radu Serban, Asher Elmquist, Aaron Young
// =============================================================================
//
// Main driver function for the hmmwv full model.
//
// The vehicle reference frame has Z up, X towards the front of the vehicle, and
// Y pointing to the left.
//
// =============================================================================

#include "chrono/core/ChRealtimeStep.h"
#include "chrono/core/ChStream.h"
#include "chrono/utils/ChUtilsInputOutput.h"

#include "chrono/physics/ChBodyEasy.h"

#include "chrono_vehicle/ChConfigVehicle.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/driver/ChDataDriver.h"
#include "chrono_vehicle/terrain/RigidTerrain.h"
#include "chrono_vehicle/wheeled_vehicle/utils/ChWheeledVehicleIrrApp.h"

#include "chrono_models/vehicle/hmmwv/HMMWV.h"

#include "chrono_thirdparty/filesystem/path.h"

#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/ChLidarSensor.h"
#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterVisualizePointCloud.h"
#include "chrono_sensor/filters/ChFilterPCfromDepth.h"
#include "chrono_sensor/filters/ChFilterSave.h"
#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/utils/ChVisualMaterialUtils.h"

#include "synchrono_interface/SynInterface.h"
#include "synchrono_interface/SynInterfaceData.h"
#include "synchrono_interface/SynInterfaceDriver.h"

#include "SynCameraComponent.h"
#include "SynTimeComponent.h"
#include "SynControlComponent.h"
#include "SynLidarComponent.h"

using namespace chrono;
using namespace chrono::geometry;
using namespace chrono::irrlicht;
using namespace chrono::vehicle;
using namespace chrono::vehicle::hmmwv;
using namespace chrono::sensor;

using namespace synchrono::interface;

// =============================================================================

// Initial vehicle location and orientation
ChVector<> initLoc(0, -25.0, 1.0);
ChQuaternion<> initRot(Q_from_AngAxis(CH_C_PI / 3, {0, 0, 1}));
// ChQuaternion<> initRot(0.866025, 0, 0, 0.5);
// ChQuaternion<> initRot(0.7071068, 0, 0, 0.7071068);
// ChQuaternion<> initRot(0.25882, 0, 0, 0.965926);
// ChQuaternion<> initRot(0, 0, 0, 1);

enum DriverMode { DEFAULT, RECORD, PLAYBACK };
DriverMode driver_mode = DEFAULT;

// Visualization type for vehicle parts (PRIMITIVES, MESH, or NONE)
VisualizationType chassis_vis_type = VisualizationType::MESH;
VisualizationType suspension_vis_type = VisualizationType::PRIMITIVES;
VisualizationType steering_vis_type = VisualizationType::PRIMITIVES;
VisualizationType wheel_vis_type = VisualizationType::MESH;

// Collision type for chassis (PRIMITIVES, MESH, or NONE)
ChassisCollisionType chassis_collision_type = ChassisCollisionType::NONE;

// Type of powertrain model (SHAFTS, SIMPLE)
// PowertrainModelType powertrain_model = PowertrainModelType::SHAFTS;

// Drive type (FWD)
// DrivelineType drive_type = DrivelineType::FWD;

// Type of tire model (RIGID, TMEASY)
TireModelType tire_model = TireModelType::RIGID;

// Rigid terrain
RigidTerrain::Type terrain_model = RigidTerrain::BOX;  // RigidTerrain::BOX,MESH;
double terrainHeight = 0;                              // terrain height (FLAT terrain only)
double terrainLength = 100.0;                          // size in X direction
double terrainWidth = 100.0;                           // size in Y direction

// Point on chassis tracked by the camera
ChVector<> trackPoint(0.0, 0.0, 1.75);

// Contact method
ChMaterialSurface::ContactMethod contact_method = ChMaterialSurface::SMC;
bool contact_vis = false;

// Simulation step sizes
double step_size = 2e-3;
double tire_step_size = step_size;

// Simulation end time
double t_end = 1000;

// Time interval between two render frames
double render_step_size = 1.0 / 50;  // FPS = 50

// Output directories
const std::string out_dir = GetChronoOutputPath() + "hmmwv";
const std::string pov_dir = out_dir + "/POVRAY";

// Debug logging
bool debug_output = false;
double debug_step_size = 1.0 / 1;  // FPS = 1

// File path of the cone positions
// const std::string blue_path_file = "paths/blue_straight.txt";
// const std::string yellow_path_file = "paths/yellow_straight.txt";
const std::string blue_path_file = "paths/inner_circle.txt";
const std::string yellow_path_file = "paths/outer_circle.txt";

// POV-Ray output
bool povray_output = false;

int num_gpus_to_use = 1;

// =============================================================================

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage :: " << std::string(argv[0]) << " <port number>" << std::endl;
        return 1;
    }
    std::string port = std::string(argv[1]);

    GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";

    SetChronoDataPath(SYNCHRONO_DATA_DIR);
    vehicle::SetDataPath(SYNCHRONO_VEHICLE_DATA_DIR);
    SetDataInterfacePath(SYNCHRONO_INTERFACE_DATA_DIR);
    // --------------
    // Create systems
    // --------------

    // Create the hmmwv vehicle, set parameters, and initialize
    HMMWV_Full my_hmmwv;
    my_hmmwv.SetContactMethod(contact_method);
    my_hmmwv.SetChassisCollisionType(chassis_collision_type);
    my_hmmwv.SetChassisFixed(false);
    my_hmmwv.SetInitPosition(ChCoordsys<>(initLoc, initRot));
    // my_hmmwv.SetPowertrainType(powertrain_model);
    // my_hmmwv.SetDriveType(drive_type);
    my_hmmwv.SetTireType(tire_model);
    my_hmmwv.SetTireStepSize(tire_step_size);
    my_hmmwv.SetVehicleStepSize(step_size);
    my_hmmwv.Initialize();

    VisualizationType tire_vis_type = VisualizationType::NONE;  // : VisualizationType::PRIMITIVES;

    my_hmmwv.SetChassisVisualizationType(chassis_vis_type);
    my_hmmwv.SetSuspensionVisualizationType(suspension_vis_type);
    my_hmmwv.SetSteeringVisualizationType(steering_vis_type);
    my_hmmwv.SetWheelVisualizationType(wheel_vis_type);
    my_hmmwv.SetTireVisualizationType(tire_vis_type);

    // Create the terrain
    RigidTerrain terrain(my_hmmwv.GetSystem());

    std::shared_ptr<RigidTerrain::Patch> patch;
    auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
    switch (terrain_model) {
        case RigidTerrain::BOX:
            patch = terrain.AddPatch(ChCoordsys<>(ChVector<>(0, 0, terrainHeight - 5), QUNIT),
                                     ChVector<>(terrainLength, terrainWidth, 10));
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/concrete.jpg"), 200, 200);

            if (true) {
                vis_mat->SetKdTexture(vehicle::GetDataFile("terrain/textures/concrete.jpg"));
            } else {
                vis_mat->SetAmbientColor({0.f, 0.f, 0.f});
                vis_mat->SetDiffuseColor({(float)ChRandom(), (float)ChRandom(), (float)ChRandom()});
                vis_mat->SetSpecularColor({.2f, .2f, .2f});
            }
            std::dynamic_pointer_cast<ChVisualization>((patch->GetGroundBody()->GetAssets()[0]))
                ->material_list.push_back(vis_mat);
            break;
        case RigidTerrain::HEIGHT_MAP:
            patch = terrain.AddPatch(CSYSNORM, vehicle::GetDataFile("terrain/height_maps/test64.bmp"), "test64", 128,
                                     128, 0, 4);
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 16, 16);
            break;
        case RigidTerrain::MESH:
            patch = terrain.AddPatch(CSYSNORM, vehicle::GetDataFile("terrain/meshes/test.obj"), "test_mesh");
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 100, 100);
            break;
    }
    patch->SetContactFrictionCoefficient(0.9f);
    patch->SetContactRestitutionCoefficient(0.01f);
    patch->SetContactMaterialProperties(2e7f, 0.3f);
    patch->SetColor(ChColor(0.8f, 0.8f, 0.5f));
    terrain.Initialize();

    // Create the vehicle Irrlicht interface
    ChWheeledVehicleIrrApp app(&my_hmmwv.GetVehicle(), L"hmmwv Demo");
    app.SetSkyBox();
    app.AddTypicalLights(irr::core::vector3df(30.f, -30.f, 100.f), irr::core::vector3df(30.f, 50.f, 100.f), 250, 130);
    app.SetChaseCamera(trackPoint, 6.0, 0.5);
    app.SetTimestep(step_size);
    app.AssetBindAll();
    app.AssetUpdateAll();

    // Create cones
    auto blue_path = ChBezierCurve::read(GetDataInterfaceFile(blue_path_file));
    auto yellow_path = ChBezierCurve::read(GetDataInterfaceFile(yellow_path_file));

    auto green_mesh = chrono_types::make_shared<ChTriangleMeshConnected>();
    green_mesh->LoadWavefrontMesh(GetDataInterfaceFile("cones/green_cone_big.obj"), false, true);
    // green_mesh->LoadWavefrontMesh(GetDataInterfaceFile("cones/blue_cone.obj"), false, true);
    green_mesh->Transform(ChVector<>(0, 0, 0), ChMatrix33<>(1));  // scale to a different size
    green_mesh->RepairDuplicateVertexes(1e-9);

    auto asset_green_mesh = chrono_types::make_shared<ChTriangleMeshShape>();
    asset_green_mesh->SetMesh(green_mesh);
    asset_green_mesh->SetBackfaceCull(true);

    auto red_mesh = chrono_types::make_shared<ChTriangleMeshConnected>();
    // red_mesh->LoadWavefrontMesh(GetDataInterfaceFile("cones/yellow_cone.obj"), false, true);
    red_mesh->LoadWavefrontMesh(GetDataInterfaceFile("cones/red_cone_big.obj"), false, true);
    red_mesh->Transform(ChVector<>(0, 0, 0), ChMatrix33<>(1));  // scale to a different size
    red_mesh->RepairDuplicateVertexes(1e-9);

    auto asset_red_mesh = chrono_types::make_shared<ChTriangleMeshShape>();
    asset_red_mesh->SetMesh(red_mesh);
    asset_red_mesh->SetBackfaceCull(true);

    std::vector<ChVector<float>> blue_cones;
    std::vector<ChVector<float>> yellow_cones;

    for (size_t i = 0; i < blue_path->getNumPoints(); i++) {
        ChVector<> pb = blue_path->getPoint(i);
        ChVector<> py = yellow_path->getPoint(i);

        auto red = chrono_types::make_shared<ChBodyAuxRef>();
        red->SetMass(1000);
        red->SetFrame_REF_to_abs(ChFrame<>(pb, Q_from_AngAxis(CH_C_PI / 2.0, {1, 0, 0})));
        // red->GetCollisionModel()->ClearModel();
        // red->GetCollisionModel()->AddTriangleMesh(red_mesh, false, false, VNULL, ChMatrix33<>(1), 0.005);
        // red->GetCollisionModel()->BuildModel();
        red->SetCollide(false);
        red->AddAsset(asset_red_mesh);
        red->SetBodyFixed(true);

        auto green = chrono_types::make_shared<ChBodyAuxRef>();
        green->SetMass(1000);
        green->SetFrame_REF_to_abs(ChFrame<>(py, Q_from_AngAxis(CH_C_PI / 2.0, {1, 0, 0})));
        // green->GetCollisionModel()->ClearModel();
        // green->GetCollisionModel()->AddTriangleMesh(green_mesh, false, false, VNULL, ChMatrix33<>(1), 0.005);
        // green->GetCollisionModel()->BuildModel();
        green->SetCollide(false);
        green->AddAsset(asset_green_mesh);
        green->SetBodyFixed(true);

        my_hmmwv.GetSystem()->Add(red);
        my_hmmwv.GetSystem()->Add(green);
    }

    // -----------------
    // Initialize output
    // -----------------

    if (!filesystem::create_directory(filesystem::path(out_dir))) {
        std::cout << "Error creating directory " << out_dir << std::endl;
        return 1;
    }
    if (povray_output) {
        if (!filesystem::create_directory(filesystem::path(pov_dir))) {
            std::cout << "Error creating directory " << pov_dir << std::endl;
            return 1;
        }
        terrain.ExportMeshPovray(out_dir);
    }

    std::string driver_file = out_dir + "/driver_inputs.txt";
    utils::CSV_writer driver_csv(" ");

    // ------------------------
    // Create the driver system
    // ------------------------

    // Create the interactive driver system

    // Set the time response for steering and throttle keyboard inputs.
    double steering_time = 1.0;  // time to go from 0 to +1 (or from 0 to -1)
    double throttle_time = 1.0;  // time to go from 0 to +1
    double braking_time = 0.3;   // time to go from 0 to +1

    SynInterfaceDriver driver(my_hmmwv.GetVehicle());

    driver.SetSteeringDelta(render_step_size / steering_time);
    driver.SetThrottleDelta(render_step_size / throttle_time);
    driver.SetBrakingDelta(render_step_size / braking_time);

    // If in playback mode, attach the data file to the driver system and
    // force it to playback the driver inputs.
    if (driver_mode == PLAYBACK) {
        // driver.SetInputDataFile(driver_file);
        // driver.SetInputMode(ChIrrGuiDriver::DATAFILE);
    }

    driver.Initialize();

    // ---------------
    // Simulation loop
    // ---------------

    if (debug_output) {
        GetLog() << "\n\n============ System Configuration ============\n";
        my_hmmwv.LogHardpointLocations();
    }

    // output vehicle mass
    std::cout << "VEHICLE MASS: " << my_hmmwv.GetVehicle().GetVehicleMass() << std::endl;

    // Number of simulation steps between miscellaneous events
    int render_steps = (int)std::ceil(render_step_size / step_size);
    int debug_steps = (int)std::ceil(debug_step_size / step_size);

    // Initialize simulation frame counter and simulation time
    ChRealtimeStepTimer realtime_timer;
    int step_number = 0;
    int render_frame = 0;
    double time = 0;

    if (contact_vis) {
        app.SetSymbolscale(1e-4);
        app.SetContactsDrawMode(ChIrrTools::eCh_ContactsDrawMode::CONTACT_FORCES);
    }

    // create the sensor manager and a camera
    auto manager = chrono_types::make_shared<ChSensorManager>(my_hmmwv.GetSystem());

    // set lights
    // manager->scene->AddPointLight({50, 50, 50}, {1, 1, 1}, 250);
    manager->scene->AddPointLight({-50, 50, 50}, {1, 1, 1}, 250);
    manager->scene->AddPointLight({-50, -50, 50}, {1, 1, 1}, 250);
    manager->scene->AddPointLight({50, -50, 50}, {1, 1, 1}, 250);
    manager->scene->AddPointLight({50, 50, -50}, {1, 1, 1}, 250);
    // manager->scene->AddPointLight({-100, -100, -100}, {1, 1, 1}, 500);

    auto cam = chrono_types::make_shared<ChCameraSensor>(
        my_hmmwv.GetChassisBody(),                                           // body camera is attached to
        30,                                                                  // update rate in Hz
        chrono::ChFrame<double>({-8, 0, 3}, Q_from_AngAxis(.2, {0, 1, 0})),  // offset pose
        1280,                                                                // image width
        720,                                                                 // image height
        CH_C_PI / 3, 9 / 16. * CH_C_PI / 3);
    cam->SetName("camera_sensor_1");
    manager->AddSensor(cam);

    cam->PushFilter(chrono_types::make_shared<ChFilterVisualize>("Camera 1"));
    cam->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>("Camera 1"));

    auto cam2 = chrono_types::make_shared<ChCameraSensor>(
        my_hmmwv.GetChassisBody(),                                            // body camera is attached to
        30,                                                                   // update rate in Hz
        chrono::ChFrame<double>({1, 0, .875}, Q_from_AngAxis(0, {1, 0, 0})),  // offset pose
        1280,                                                                 // image width
        720,                                                                  // image height
        CH_C_PI / 3, 9 / 16. * CH_C_PI / 3);
    cam2->SetName("camera_sensor_2");
    manager->AddSensor(cam2);

    cam2->PushFilter(chrono_types::make_shared<ChFilterVisualize>("Camera 2"));
    cam2->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>("Camera 2"));
    //
    // auto lidar = chrono_types::make_shared<ChLidarSensor>(
    //     my_hmmwv.GetChassisBody(),                                          // body lidar is attached to
    //     10,                                                                 // scanning rate in Hz
    //     chrono::ChFrame<double>({-8, 0, 1}, Q_from_AngAxis(0, {0, 1, 0})),  // offset pose
    //     1000,                                                               // number of horizontal samples
    //     50,                                                                 // number of vertical channels
    //     CH_C_PI,                                                            // horizontal field of view
    //     CH_C_PI / 6.                                                        // vertical field of view
    // );
    // lidar->SetName("lidar");
    // manager->AddSensor(lidar);
    //
    // lidar->PushFilter(chrono_types::make_shared<ChFilterVisualizePointCloud>("lidar"));
    // lidar->PushFilter(chrono_types::make_shared<ChFilterPCfromDepth>("lidar"));
    // lidar->PushFilter(chrono_types::make_shared<ChFilterXYZIAccess>());

    double wall_time = 0;

    int num_camera_updates = 0;

    // Create the interface
    SynInterface interface(port);

    try {
        interface.Add(chrono_types::make_shared<SynCameraComponent>(cam->GetName(), cam->GetUpdateRate(), cam));
        interface.Add(chrono_types::make_shared<SynCameraComponent>(cam2->GetName(), cam2->GetUpdateRate(), cam2));
        interface.Add(chrono_types::make_shared<SynTimeComponent>("Time", my_hmmwv.GetSystem()));
        interface.Add(chrono_types::make_shared<SynControlComponent>("Control", driver));
        // interface.Add(chrono_types::make_shared<SynLidarComponent>(lidar->GetName(), lidar->GetUpdateRate(), lidar));
    } catch (const char* err) {
        std::cerr << err << std::endl;
        return -1;
    }

    while (app.GetDevice()->run()) {
        std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

        time = my_hmmwv.GetSystem()->GetChTime();

        // End simulation
        if (time >= t_end)
            break;

        // Render scene and output POV-Ray data
        if (step_number % render_steps == 0) {
            app.BeginScene(true, true, irr::video::SColor(255, 140, 161, 192));
            app.DrawAll();
            app.EndScene();

            if (povray_output) {
                char filename[100];
                sprintf(filename, "%s/data_%03d.dat", pov_dir.c_str(), render_frame + 1);
                utils::WriteShapesPovray(my_hmmwv.GetSystem(), filename);
            }

            render_frame++;
        }

        // Debug logging
        if (debug_output && step_number % debug_steps == 0) {
            GetLog() << "\n\n============ System Information ============\n";
            GetLog() << "Time = " << time << "\n\n";
            my_hmmwv.DebugLog(OUT_SPRINGS | OUT_SHOCKS | OUT_CONSTRAINTS);
        }

        // Collect output data from modules (for inter-module communication)
        ChDriver::Inputs driver_inputs = driver.GetInputs();

        // Driver output
        if (driver_mode == RECORD) {
            driver_csv << time << driver_inputs.m_steering << driver_inputs.m_throttle << driver_inputs.m_braking
                       << std::endl;
        }

        // Update modules (process inputs from other modules)
        interface.Synchronize(time);
        terrain.Synchronize(time);
        my_hmmwv.Synchronize(time, driver_inputs, terrain);

        app.Synchronize(driver.GetInputModeAsString(), driver_inputs);
        driver.Synchronize(time);

        // Advance simulation for one timestep for all modules
        double step = realtime_timer.SuggestSimulationStep(step_size);

        terrain.Advance(step);
        my_hmmwv.Advance(step);

        app.Advance(step);
        driver.Advance(step);

        manager->Update();
        // manager->Render();
        // }

        // Increment frame number
        step_number++;

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
        wall_time += t.count();

        if (fmod(time, 1.0) <= 1e-3) {
            // std::cout << "Wall Time :: " << wall_time << std::endl;
            wall_time = 0;
        }
    }

    if (driver_mode == RECORD) {
        driver_csv.write_to_file(driver_file);
    }

    return 0;
}
