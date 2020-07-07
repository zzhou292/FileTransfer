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

#include "chrono/core/ChVector.h"
#include "chrono_vehicle/wheeled_vehicle/ChWheeledVehicle.h"
#include "chrono_models/vehicle/hmmwv/HMMWV.h"

#include "chrono_thirdparty/filesystem/path.h"
#include <chrono>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;

// =============================================================================
// Simulation parameters
const double HEARTBEAT = 0.01;
const double SIM_EPSILON = 1e-9;
const bool use_SCM_terrain = true;

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();

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

    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    // Timing
    double wall_time = 0;

    // ------------------
    // Agent Initialization
    // ------------------

    SynMPIManager mpi_manager(argc, argv);
    int rank = mpi_manager.GetRank();
    int num_ranks = mpi_manager.GetNumRanks();

    auto agent = chrono_types::make_shared<SynFullHMMWVAgent>(rank);
    mpi_manager.AddAgent(agent, rank);

    // This adds the zombie agents
    for (int i = 1; i < num_ranks; i++) {
        if (i != rank) {
            mpi_manager.AddAgent(chrono_types::make_shared<SynFullHMMWVAgent>(i, agent->GetSystem()), i);
        }
    }

    std::shared_ptr<SynSCMTerrain> SCM_terrain;

    if (rank != 0) {
        // Use up more of the mesh by not placing vehicles in the middle
        ChVector<> offset(-size_x / 2 + 5, 0, 0);

        // Initializing terrain and vehicles
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

        agent->SetUseLuggedTires(rank == 3);
        agent->InitializeVehicle(ChCoordsys<>(init_loc, init_rot));

        if (use_SCM_terrain) {
            SCMParameters params;
            params.InitializeParametersAsMid();
            auto SCM_terrain = chrono_types::make_shared<SynSCMTerrain>(
                agent->GetSystem(), size_x, size_y, divs_per_units * size_x, divs_per_units * size_y, &params);

            if (ENABLE_MOVING_PATCH) {
                // Right side for no particular reason, all wheels should have same radius
                double tire_rad =
                    agent->GetHMMWV()->GetVehicle().GetWheel(0, chrono::vehicle::VehicleSide::RIGHT)->GetRadius();

                for (auto& axle : agent->GetHMMWV()->GetVehicle().GetAxles()) {
                    auto left_wheel = axle->m_wheels[0]->GetSpindle();
                    auto right_wheel = axle->m_wheels[1]->GetSpindle();

                    SCM_terrain->GetSCMTerrain()->AddMovingPatch(left_wheel, ChVector<>(0, 0, 0), tire_rad, 1.0);
                    SCM_terrain->GetSCMTerrain()->AddMovingPatch(right_wheel, ChVector<>(0, 0, 0), tire_rad, 1.0);
                }
            }

            // Add texture for the terrain
#ifdef SENSOR
            auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
            vis_mat->SetSpecularColor({.1f, .1f, .1f});
            vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
            SCM_terrain->GetSCMTerrain()->GetMesh()->material_list.push_back(vis_mat);
#endif  // SENSOR

            agent->SetTerrain(SCM_terrain);
        } else {
            auto rigid_terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), 0, size_x, size_y);
            agent->SetTerrain(rigid_terrain);
        }

        // Make the path and parameters for the ACC driver that guides the agent along the path
        double target_speed = 10;  // 1 + 10 * (rank == 2);
        double target_following_time = 1;
        double target_min_distance = 10;
        double current_distance = 100;
        bool isPathClosed = false;

        auto path = chrono_types::make_shared<ChBezierCurve>(curve_pts);
        auto driver = chrono_types::make_shared<ChPathFollowerACCDriver>(
            agent->GetVehicle(), path, "Box path", target_speed, target_following_time, target_min_distance,
            current_distance, isPathClosed);

        double Kp = 0.4;
        double Ki = 0;
        double Kd = 0;
        driver->GetSpeedController().SetGains(Kp, Ki, Kd);
        driver->GetSteeringController().SetGains(Kp, Ki, Kd);
        driver->GetSteeringController().SetLookAheadDistance(5);

        auto brain = chrono_types::make_shared<ACCBrain>(rank, driver, agent->GetVehicle());
        agent->SetBrain(brain);

        auto vis_manager = chrono_types::make_shared<SynVisManager>();
        agent->AttachVisManager(vis_manager);

#ifdef IRRLICHT
        double IRR_STEP_SIZE = 0.001;
        if (cli.HasValueInVector<int>("irr", rank)) {
            auto irr_vis = chrono_types::make_shared<SynIrrVehicleVis>(driver, IRR_STEP_SIZE);
            irr_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle());
            irr_vis->SetSave(cli.GetAsType<bool>("irr_save"));
            irr_vis->SetVisualize(cli.GetAsType<bool>("irr_vis"));
            vis_manager->AttachVis(irr_vis);
        }
#endif  // IRRLICHT

#ifdef SENSOR
        if (cli.HasValueInVector<int>("sens", rank)) {
            double fps = 30;

            auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>(SynSenVehicleVis::CAMERA);

            auto manager = chrono_types::make_shared<ChSensorManager>(agent->GetSystem());
            manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 2000);
            manager->scene->AddPointLight({-100, 100, 100}, {1, 1, 1}, 2000);
            sen_vis->SetSensorManager(manager);

            auto origin = chrono_types::make_shared<ChBody>();
            origin->SetBodyFixed(true);
            agent->GetSystem()->AddBody(origin);

            ChVector<> camera_loc(cam_x, cam_y, 65);

            // Rotations to get a nice angle
            ChQuaternion<> rotation = QUNIT;
            // ChQuaternion<> qA = Q_from_AngAxis(35 * CH_C_DEG_TO_RAD, VECT_Y);
            // ChQuaternion<> qB = Q_from_AngAxis(135 * CH_C_DEG_TO_RAD, VECT_Z);
            ChQuaternion<> qA = Q_from_AngAxis(90 * CH_C_DEG_TO_RAD, VECT_Y);
            ChQuaternion<> qB = Q_from_AngAxis(180 * CH_C_DEG_TO_RAD, VECT_Z);
            rotation = rotation >> qA >> qB;

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

            std::string file_path = std::string("SENSOR_OUTPUT/Sedan") + std::to_string(rank) + std::string("/");
            if (cli.GetAsType<bool>("sens_save")) {
                intersection_camera->PushFilter(chrono_types::make_shared<ChFilterSave>(file_path));
            }

            sen_vis->SetSensor(intersection_camera);
            vis_manager->AttachVis(sen_vis);

            // auto vis_camera_vis = chrono_types::make_shared<SynSenVehicleVis>();
            // // auto vis_camera = chrono_types::make_shared<chrono::sensor::ChCameraSensor>(
            // //     agent->GetVehicle().GetChassisBody(), 30,
            // //     chrono::ChFrame<double>({1.5, 4, .5}, Q_from_AngAxis(-CH_C_PI / 2, {0, 0, 1})), CAM_RES_WIDTH,
            // //     CAM_RES_HEIGHT, CH_C_PI / 3);
            // auto vis_camera = chrono_types::make_shared<chrono::sensor::ChCameraSensor>(
            //     agent->GetVehicle().GetChassisBody(), fps,
            //     chrono::ChFrame<double>({-8, 0, 3}, Q_from_AngAxis(.2, {0, 1, 0})), CAM_RES_WIDTH, CAM_RES_HEIGHT,
            //     CH_C_PI / 3, 1 / fps, 0);
            // vis_camera->SetName("Chase Cam");
            // vis_camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());
            // if (cli.GetAsType<bool>("sens_save")) {
            //     vis_camera->PushFilter(chrono_types::make_shared<ChFilterSave>(file_path));
            // }
            // vis_camera_vis->SetSensorManager(manager);
            // vis_camera_vis->SetSensor(vis_camera);
            // vis_manager->AttachVis(vis_camera_vis);
        }
#endif  // SENSOR
    }

    mpi_manager.Barrier();
    mpi_manager.Initialize();
    mpi_manager.Barrier();

    int step = 0;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Simulation Loop
    while (step * HEARTBEAT <= T_END) {
        mpi_manager.Advance(step * HEARTBEAT);

        mpi_manager.Synchronize();
        mpi_manager.Broadcast();
        mpi_manager.Update();

        // increment the step
        step++;

        // if (rank == 0 && fmod(step * HEARTBEAT, 1.0) <= 1e-5) {
        //     std::chrono::high_resolution_clock::time_point time_now = std::chrono::high_resolution_clock::now();
        //     auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(time_now - start);
        //     std::cout << "Time: " << (step * HEARTBEAT)
        //               << ", fraction: " << ((time_span.count() / 1e6) / (step * HEARTBEAT)) << std::endl;
        // }
    }

    if (rank == 0) {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "real time: " << (time_span.count() / 1e6) / T_END << std::endl;
    }

    std::ofstream output_file("./example.txt");
    // std::copy(
    //     adv_times.rbegin(), adv_times.rend(),
    //     std::ostream_iterator<int>(std::string("./adv_times") + std::to_string(rank) + std::string(".txt"), "\n"));

    if (rank == 1 && use_SCM_terrain) {
        // std::string mesh_path("syn_mesh.obj");
        // std::shared_ptr<geometry::ChTriangleMeshConnected> mesh = SCM_terrain->GetSCMTerrain()->GetMesh()->GetMesh();
        // std::vector<geometry::ChTriangleMeshConnected> vec;
        // vec.push_back(*mesh);
        // geometry::ChTriangleMeshConnected::WriteWavefront(mesh_path, vec);
    }

    // std::cout << "Rank " << rank << " completed successfully" << std::endl;

    return 0;
}
