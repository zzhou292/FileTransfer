#include "synchrono/SynMPIManager.h"
#include "synchrono/SynVisManager.h"
#include "synchrono/SynFramework.h"
#include "synchrono/SynCLI.h"

#include "synchrono/brain/ACCBrain.h"
#include "synchrono/brain/TrafficLightBrain.h"
#include "synchrono/agent/SynSedanAgent.h"
#include "synchrono/agent/SynTrafficLightAgent.h"

#ifdef IRRLICHT
#include "synchrono/visualization/SynIrrVehicleVis.h"
#endif  // IRRLICHT

#ifdef SENSOR
#include "synchrono/visualization/SynSenVehicleVis.h"
#endif  // SENSOR

#include "synchrono/terrain/SynRigidTerrain.h"

#include "chrono/core/ChVector.h"
#include "chrono_thirdparty/filesystem/path.h"

#include <chrono>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();
    cli.AddOption("Demo",
                  cxxopts::Option("t,timer", "Toggle timer on", cxxopts::value<bool>()->default_value("false")));
    cli.Parse(argc, argv);
    cli.SetVerbosity(true);

    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    const double T_END = cli.GetAsType<double>("time_end");
    const double HEARTBEAT = cli.GetAsType<double>("heartbeat");
    const double STEP_SIZE = cli.GetAsType<double>("step_size");
    const bool verbose = cli.GetAsType<bool>("verbose");
    const bool use_timer = cli.GetAsType<bool>("timer");

    // For timing
    double wall_time = 0;

    // Create MPI Manager
    SynMPIManager mpi_manager(argc, argv);
    int rank = mpi_manager.GetRank();
    int num_ranks = mpi_manager.GetNumRanks();

    if (rank != 0) {
        // ====================
        // Agent Initialization
        // ====================
        auto agent = chrono_types::make_shared<SynSedanAgent>(rank);
        mpi_manager.AddAgent(agent, rank);

        for (int i = 1; i < num_ranks; i++) {
            if (i != rank)
                mpi_manager.AddAgent(chrono_types::make_shared<SynSedanAgent>(i), i);
        }

        if (rank != 0) {
            // Determine initial location and orientation depending on rank
            // TODO :: Find a better way to do this, maybe load from file or located in separate class

            // Grid of vehicles
            int col = (rank - 1) % 3;
            int row = rank / 3;

            // Box dimensions
            double length = 400;
            double width = 25;

            ChVector<double> base = ChVector<double>({-length / 2 + 5, -width / 2 + 5, 1.0});
            ChVector<double> offset = ChVector<>({30.0 * row, 5.0 * col, 0});
            ChVector<double> init_loc = base + offset;

            ChQuaternion<> initRot = ChQuaternion<>({1, 0, 0, 0});
            agent->InitializeVehicle(ChCoordsys<>(base + offset, initRot));
            auto terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), 0, length, width);
            agent->SetTerrain(terrain);

            std::vector<ChVector<double>> path_points = {init_loc + ChVector<double>(0, 0, 0),
                                                         init_loc + ChVector<double>(length, 0, 0)};
            std::shared_ptr<ChBezierCurve> path = chrono_types::make_shared<ChBezierCurve>(path_points);

            double target_speed = 10;
            double target_following_time = 1.2;
            double target_min_distance = 50;
            double current_distance = 100;
            bool isPathClosed = false;

            std::shared_ptr<ChPathFollowerACCDriver> driver = chrono_types::make_shared<ChPathFollowerACCDriver>(
                agent->GetVehicle(), path, "Straight ahead", target_speed, target_following_time, target_min_distance,
                current_distance, isPathClosed);

            double speed_Kp = 0.4;
            double speed_Ki = 0;
            double speed_Kd = 0;

            double turn_Kp = 0.4;
            double turn_Ki = 0.1;
            double turn_Kd = 0.2;
            driver->GetSpeedController().SetGains(speed_Kp, speed_Ki, speed_Kd);
            driver->GetSteeringController().SetGains(turn_Kp, turn_Ki, turn_Kd);
            driver->GetSteeringController().SetLookAheadDistance(5);

            std::shared_ptr<ACCBrain> brain = chrono_types::make_shared<ACCBrain>(rank, driver, agent->GetVehicle());
            agent->SetBrain(brain);

            std::shared_ptr<SynVisManager> vis_manager = chrono_types::make_shared<SynVisManager>();
            agent->AttachVisManager(vis_manager);
#ifdef IRRLICHT
            if (cli.HasValueInVector<int>("irr", rank)) {
                std::shared_ptr<SynIrrVehicleVis> irr_vis =
                    chrono_types::make_shared<SynIrrVehicleVis>(driver, STEP_SIZE);
                irr_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle());
                vis_manager->AttachVis(irr_vis);
            }
#endif

#ifdef SENSOR
            if (cli.HasValueInVector<int>("sens", rank)) {
                std::shared_ptr<SynSenVehicleVis> sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
                sen_vis->InitializeDefaultSensorManager(agent->GetSystem());
                sen_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle().GetChassisBody());
                if (cli.GetAsType<bool>("sens_save"))
                    sen_vis->AddFilterSave();
                if (cli.GetAsType<bool>("sens_vis"))
                    sen_vis->AddFilterVisualize();
                vis_manager->AttachVis(sen_vis);
            }
#endif
        }
    }

    mpi_manager.Barrier();
    mpi_manager.Initialize();
    mpi_manager.Barrier();

    double step = 0;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Simulation Loop
    while (step * HEARTBEAT < T_END) {
        std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

        // Advance
        mpi_manager.Advance(step * HEARTBEAT);
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        auto t = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        auto inc = t.count() / 1e6;
        wall_time += inc;
        if (verbose && inc > 1e-4) {
            std::cout << step << "." << rank << " adv: " << t.count() / 1e6 << std::endl;
        }

        // Synchronize
        mpi_manager.Synchronize();
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        t = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        inc = t.count() / 1e6;
        wall_time += inc;
        if (verbose && inc > 1e-4) {
            std::cout << step << "." << rank << " syn: " << inc << std::endl;
        }

        // Broadcast
        mpi_manager.Broadcast();
        std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
        t = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
        inc = t.count() / 1e6;
        wall_time += inc;
        if (verbose && inc > 1e-4) {
            std::cout << step << "." << rank << " bro: " << inc << std::endl;
        }

        // Update
        mpi_manager.Update();
        std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
        t = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);
        inc = t.count() / 1e6;
        wall_time += inc;
        if (verbose && inc > 1e-4) {
            std::cout << step << "." << rank << " upd: " << inc << std::endl;
        }

        // increment the step
        step++;

        std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();
        t = std::chrono::duration_cast<std::chrono::microseconds>(t5 - t0);
        wall_time += t.count() / 1e6;

        if (verbose && fmod(step * HEARTBEAT, 1.0) <= 1e-5) {
            std::cout << "Wall Time (" << rank << "):: " << wall_time << std::endl;
            wall_time = 0;
        }
    }

    if ((use_timer || verbose) && rank == 0) {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // std::cout << "Total Wall Time: " << time_span.count() / 1e6 << "." << std::endl;
        // std::cout << "Fraction of real time: " << (time_span.count() / 1e6) / T_END << std::endl;
        std::cout << "Frequency of steps [Hz]: " << step / (time_span.count() / 1e6) << std::endl;
        // std::cout << "Real time: " << (time_span.count() / 1e6) / T_END << std::endl;
    }

    return 0;
}
