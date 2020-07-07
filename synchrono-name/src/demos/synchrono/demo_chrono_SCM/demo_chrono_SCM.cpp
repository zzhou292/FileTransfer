#include <chrono>

#include "synchrono/SynMPIManager.h"
#include "synchrono/SynVisManager.h"
#include "synchrono/SynFramework.h"
#include "synchrono/SynCLI.h"

#include "synchrono/brain/NOOPBrain.h"
#include "synchrono/brain/ACCBrain.h"
#include "synchrono/agent/SynSedanAgent.h"
#include "synchrono/agent/SynHMMWVAgent.h"
#include "synchrono/agent/SynFullHMMWVAgent.h"
#include "synchrono/agent/SynTrafficLightAgent.h"

#ifdef IRRLICHT
#include "synchrono/visualization/SynIrrVehicleVis.h"
#endif  // IRRLICHT

#ifdef SENSOR
#include "synchrono/visualization/SynSenVehicleVis.h"
#endif  // SENSOR

#include "synchrono/terrain/SynSCMTerrain.h"
#include "synchrono/terrain/SynRigidTerrain.h"

#include "chrono/geometry/ChTriangleMeshConnected.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/core/ChVector.h"
#include "chrono_vehicle/wheeled_vehicle/ChWheeledVehicle.h"
#include "chrono_models/vehicle/hmmwv/HMMWV.h"

#include "chrono_thirdparty/filesystem/path.h"

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;

// =============================================================================
// Simulation parameters
const double HEARTBEAT = 0.01;
// const double STEP_SIZE = 0.001;
const double SIM_EPSILON = 1e-9;

// Default from SynVehicleAgent...
const double VEH_STEP_SIZE = 3e-3;

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    // cli.AddDefaultDemoOptions();

    cli.AddOption("Demo", cxxopts::Option("t,time", "Simulation time", cxxopts::value<double>()->default_value("10")));
    cli.AddOption("Demo", cxxopts::Option("d,divs_per_units", "Divisions per unit",
                                          cxxopts::value<double>()->default_value("20")));
    cli.AddOption("Demo", cxxopts::Option("x,size_x", "Size in the x", cxxopts::value<double>()->default_value("100")));
    cli.AddOption("Demo", cxxopts::Option("y,size_y", "Size in the y", cxxopts::value<double>()->default_value("50")));
    // Would rather have it be w and h, but h is taken by the --help flag
    cli.AddOption("Demo",
                  cxxopts::Option("width", "Camera resolution width", cxxopts::value<double>()->default_value("1280")));
    cli.AddOption(
        "Demo", cxxopts::Option("height", "Camera resolution height", cxxopts::value<double>()->default_value("720")));
    cli.AddOption("Demo", cxxopts::Option("moving_patch", "Toggle moving patches on",
                                          cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo", cxxopts::Option("cx", "Camera x loc", cxxopts::value<double>()->default_value("-15")));
    cli.AddOption("Demo", cxxopts::Option("cy", "Camera y loc", cxxopts::value<double>()->default_value("-25")));

    // Sensor options
    cli.AddOption("Sensor", cxxopts::Option("sens_fixed", "Initialize fixed cam",
                                            cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor", cxxopts::Option("sens_chase", "Initialize chase cam",
                                            cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor", cxxopts::Option("sens_save", "Toggle sensor saving on",
                                            cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor", cxxopts::Option("sens_vis", "Toggle for sensor visualizing on",
                                            cxxopts::value<bool>()->default_value("false")));

    cli.AddOption("", cxxopts::Option("h,help", "Print usage"));

    cli.Parse(argc, argv);
    cli.SetVerbosity(true);

    // Set values
    const double T_END = cli.GetAsType<double>("time");
    const double size_x = cli.GetAsType<double>("size_x");
    const double size_y = cli.GetAsType<double>("size_y");
    const double cam_x = cli.GetAsType<double>("cx");
    const double cam_y = cli.GetAsType<double>("cy");
    const double divs_per_units = cli.GetAsType<double>("divs_per_units");
    const double CAM_RES_WIDTH = cli.GetAsType<double>("width");
    const double CAM_RES_HEIGHT = cli.GetAsType<double>("height");
    const bool ENABLE_MOVING_PATCH = cli.GetAsType<bool>("moving_patch");

    std::cout << "Set CLI data" << std::endl;

    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    std::cout << "Set data paths" << std::endl;

    // -------------
    // Humvee SETUP
    // -------------

    auto other_humvee = chrono_types::make_shared<HMMWV_Full>();

    std::cout << "Made Humvee" << std::endl;

    bool use_lugged_tires = false;

    // Holdover from SynChrono
    int rank = 1;

    // Use up more of the mesh by not placing vehicles in the middle
    ChVector<> offset(-size_x / 2 + 5, 0, 0);

    ChVector<> init_loc;
    chrono::Quaternion init_rot;
    std::vector<ChVector<>> curve_pts;

    if (rank % 2 == 0) {
        // Start even vehicles in a row on the south side, driving north
        init_loc = offset + ChVector<>(0, 2.0 * (rank - 1), 0.5);
        init_rot = Q_from_AngZ(0);
        curve_pts = {init_loc, init_loc + ChVector<>(100, 0, 0)};
    } else {
        // Start odd vehicles staggered going up the west edge, driving east
        init_loc = offset + ChVector<>(2.0 * (rank - 1), -5.0 - 2.0 * (rank - 1), 0.5);
        init_rot = Q_from_AngZ(chrono::CH_C_PI / 2);
        curve_pts = {init_loc, init_loc + ChVector<>(0, 100, 0)};
    }

    std::cout << "Made path" << std::endl;

    other_humvee->SetContactMethod(ChContactMethod::SMC);
    other_humvee->SetChassisCollisionType(ChassisCollisionType::NONE);
    other_humvee->SetChassisFixed(false);
    other_humvee->SetPowertrainType(PowertrainModelType::SHAFTS);
    other_humvee->SetDriveType(DrivelineType::AWD);
    other_humvee->SetInitPosition(ChCoordsys<>(init_loc, init_rot));
    other_humvee->SetTireType(TireModelType::RIGID);
    other_humvee->SetTireStepSize(VEH_STEP_SIZE);
    other_humvee->Initialize();

    auto sys = other_humvee->GetSystem();

    std::cout << "Made ChSystem" << std::endl;

    other_humvee->SetChassisVisualizationType(VisualizationType::NONE);
    other_humvee->SetSuspensionVisualizationType(VisualizationType::MESH);
    other_humvee->SetSteeringVisualizationType(VisualizationType::NONE);

    if (use_lugged_tires) {
        other_humvee->SetWheelVisualizationType(VisualizationType::NONE);

        // Contact material properties
        float mu_t = 0.8f;
        float Y_t = 1.0e6f;
        float cr_t = 0.1f;

        auto wheel_material = chrono_types::make_shared<ChMaterialSurfaceSMC>();
        wheel_material->SetFriction(mu_t);
        wheel_material->SetYoungModulus(Y_t);
        wheel_material->SetRestitution(cr_t);

        for (auto& axle : other_humvee->GetVehicle().GetAxles()) {
            auto wheelBodyL = axle->m_wheels[0]->GetSpindle();
            SynHMMWVAgent::CreateLuggedGeometry(wheelBodyL, wheel_material);

            auto wheelBodyR = axle->m_wheels[1]->GetSpindle();
            SynHMMWVAgent::CreateLuggedGeometry(wheelBodyR, wheel_material);
        }

        // m_tire_vis_file = "hmmwv/lugged_wheel.obj";
    } else {
        other_humvee->SetWheelVisualizationType(VisualizationType::MESH);
        other_humvee->SetTireVisualizationType(VisualizationType::MESH);
    }

    std::cout << "Made Humvee" << std::endl;

    // -------------
    // TERRAIN SETUP
    // -------------
    bool auto_refine = false;
    bool bulldoze = false;

    auto terrain = chrono_types::make_shared<vehicle::SCMDeformableTerrain>(other_humvee->GetSystem());

    terrain->Initialize(0, size_x, size_y, divs_per_units * size_x, divs_per_units * size_y);

    // Softer parameters
    terrain->SetSoilParameters(2e6,   // Bekker Kphi
                               0,     // Bekker Kc
                               1.1,   // Bekker n exponent
                               0,     // Mohr cohesive limit (Pa)
                               30,    // Mohr friction limit (degrees)
                               0.01,  // Janosi shear coefficient (m)
                               4e7,   // Elastic stiffness (Pa/m), before plastic yield, must be > Kphi
                               3e4);  // Damping (Pa s/m), proportional to negative vertical speed (optional)

    // Hard parameters
    // terrain->SetSoilParameters(5301e3, 102e3, 0.793, 1.3e3, 31.1, 2e-2, 4e8, 3e4);

    terrain->SetBulldozingFlow(bulldoze);
    if (bulldoze) {
        terrain->SetBulldozingParameters(
            55,   // angle of friction for erosion of displaced material at the border of the rut
            1,    // displaced material vs downward pressed material.
            5,    // number of erosion refinements per timestep
            10);  // number of concentric vertex selections subject to erosion
    }

    // Turn on the automatic level of detail refinement, so a coarse terrain mesh
    // is automatically improved by adding more points under the wheel contact patch:
    terrain->SetAutomaticRefinement(auto_refine);
    if (auto_refine) {
        terrain->SetAutomaticRefinementResolution(0.3);
    }

    terrain->SetPlotType(vehicle::SCMDeformableTerrain::PLOT_SINKAGE, 0, 0.1);
    terrain->GetMesh()->SetWireframe(true);

    std::cout << "Made terrain" << std::endl;

    // Add texture for the terrain
#ifdef SENSOR
    auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
    vis_mat->SetSpecularColor({.1f, .1f, .1f});
    vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
    terrain->GetMesh()->material_list.push_back(vis_mat);
#endif  // SENSOR

    if (ENABLE_MOVING_PATCH) {
        // Right side for no particular reason, all wheels should have same radius
        double tire_rad = other_humvee->GetVehicle().GetWheel(0, chrono::vehicle::VehicleSide::RIGHT)->GetRadius();

        for (auto& axle : other_humvee->GetVehicle().GetAxles()) {
            auto left_wheel = axle->m_wheels[0]->GetSpindle();
            auto right_wheel = axle->m_wheels[1]->GetSpindle();

            terrain->AddMovingPatch(left_wheel, ChVector<>(0, 0, 0), tire_rad, 1.0);
            terrain->AddMovingPatch(right_wheel, ChVector<>(0, 0, 0), tire_rad, 1.0);
        }
    }

    // PATH SETUP
    double target_speed = 0;
    double target_following_time = 1;
    double target_min_distance = 10;
    double current_distance = 100;
    bool isPathClosed = false;

    auto other_path = chrono_types::make_shared<ChBezierCurve>(curve_pts);
    auto other_driver = chrono_types::make_shared<ChPathFollowerACCDriver>(
        other_humvee->GetVehicle(), other_path, "Box path", target_speed, target_following_time, target_min_distance,
        current_distance, isPathClosed);

    double Kp = 0.4;
    double Ki = 0;
    double Kd = 0;
    other_driver->GetSpeedController().SetGains(Kp, Ki, Kd);
    other_driver->GetSteeringController().SetGains(Kp, Ki, Kd);
    other_driver->GetSteeringController().SetLookAheadDistance(5);

    // Something for Irrlicht here?

#ifdef SENSOR
    auto file_path = std::string("SENSOR_OUTPUT/Sedan1/");
    bool use_fixed = cli.GetAsType<bool>("sens_fixed");
    bool use_chase = cli.GetAsType<bool>("sens_chase");

    auto manager = chrono_types::make_shared<ChSensorManager>(other_humvee->GetSystem());

    if (use_fixed || use_chase) {
        manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 2000);
        manager->scene->AddPointLight({-100, 100, 100}, {1, 1, 1}, 2000);

        if (use_fixed) {
            auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>(SynSenVehicleVis::CAMERA);

            auto origin = chrono_types::make_shared<ChBody>();
            origin->SetBodyFixed(true);
            other_humvee->GetSystem()->AddBody(origin);

            ChVector<> camera_loc(cam_x, cam_y, 25);

            // Rotations to get a nice angle
            ChQuaternion<> rotation = QUNIT;
            ChQuaternion<> qA = Q_from_AngAxis(35 * CH_C_DEG_TO_RAD, VECT_Y);
            ChQuaternion<> qB = Q_from_AngAxis(135 * CH_C_DEG_TO_RAD, VECT_Z);
            // ChQuaternion<> qA = Q_from_AngAxis(90 * CH_C_DEG_TO_RAD, VECT_Y);
            // ChQuaternion<> qB = Q_from_AngAxis(180 * CH_C_DEG_TO_RAD, VECT_Z);
            rotation = rotation >> qA >> qB;

            double fps = 30;

            auto intersection_camera = chrono_types::make_shared<chrono::sensor::ChCameraSensor>(
                origin,                                         // body camera is attached to
                fps,                                            // update rate in Hz
                chrono::ChFrame<double>(camera_loc, rotation),  // offset pose
                CAM_RES_WIDTH,                                  // image width
                CAM_RES_HEIGHT,                                 // image height
                CH_C_PI / 3, 1 / fps, 0);

            intersection_camera->SetName("Intersection Cam");
            intersection_camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());

            if (cli.GetAsType<bool>("sens_vis")) {
                intersection_camera->PushFilter(
                    chrono_types::make_shared<ChFilterVisualize>(CAM_RES_WIDTH, CAM_RES_HEIGHT));
            }

            if (cli.GetAsType<bool>("sens_save")) {
                intersection_camera->PushFilter(chrono_types::make_shared<ChFilterSave>(file_path));
            }

            manager->AddSensor(intersection_camera);
        }

        if (use_chase) {
            double fps = 30;

            auto chase_camera = chrono_types::make_shared<chrono::sensor::ChCameraSensor>(
                other_humvee->GetVehicle().GetChassisBody(), fps,
                chrono::ChFrame<double>({1.5, 4, .5}, Q_from_AngAxis(-CH_C_PI / 2, {0, 0, 1})), CAM_RES_WIDTH,
                CAM_RES_HEIGHT, CH_C_PI / 3, 1 / fps, 0);

            chase_camera->SetName("Chase Cam");
            chase_camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());

            if (cli.GetAsType<bool>("sens_save")) {
                chase_camera->PushFilter(chrono_types::make_shared<ChFilterSave>(file_path));
            }

            manager->AddSensor(chase_camera);
        }
    }
#endif  // SENSOR

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    while (sys->GetChTime() <= T_END) {
        double time = sys->GetChTime();

#ifdef SENSOR
        manager->Update();
#endif  // SENSOR

        ChDriver::Inputs other_inputs = other_driver->GetInputs();

        double o_throttle = other_driver->GetThrottle();
        double o_steering = other_driver->GetSteering();
        double o_braking = other_driver->GetBraking();

        other_driver->Synchronize(time);
        terrain->Synchronize(time);
        other_humvee->Synchronize(time, other_inputs, *terrain);

        other_driver->Advance(VEH_STEP_SIZE);
        terrain->Advance(VEH_STEP_SIZE);
        other_humvee->Advance(VEH_STEP_SIZE);

        sys->DoStepDynamics(VEH_STEP_SIZE);

        if (fmod(time, 1.0) <= 1e-2) {
            auto time_now = std::chrono::high_resolution_clock::now();
            auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - start);
            std::cout << "Time: " << time << ", fraction: " << ((time_span.count() / 1e3) / time) << std::endl;
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "real time: " << (time_span.count() / 1e3) / T_END << std::endl;

    std::string mesh_path("ch_mesh.obj");
    std::shared_ptr<geometry::ChTriangleMeshConnected> mesh = terrain->GetMesh()->GetMesh();
    std::vector<geometry::ChTriangleMeshConnected> vec;
    vec.push_back(*mesh);
    geometry::ChTriangleMeshConnected::WriteWavefront(mesh_path, vec);

    return 0;
}
