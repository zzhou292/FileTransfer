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
#include "chrono/physics/ChSystemSMC.h"
#include "chrono_thirdparty/filesystem/path.h"

#include <chrono>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;

// =============================================================================
const double T_END = 20.0;
const double HEARTBEAT = 0.001;
const double STEP_SIZE = 0.001;
// const double SIM_EPSILON = 1e-9;

const double BCAST_RADIUS = 5;
const bool verbose = false;

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();
    cli.Parse(argc, argv);
    cli.SetVerbosity(true);

    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    if (argc != 2) {
        std::cout << "Usage :: demo_chrono_platoon <num vehicles>" << std::endl;
        return -1;
    }
    int num_vehs = std::stoi(std::string(argv[1]));

    // For timing
    double wall_time = 0;

    std::vector<std::shared_ptr<SynSedanAgent>> agent_list(num_vehs);

    for (int i = 0; i < num_vehs; i++) {
        std::shared_ptr<SynSedanAgent> agent = chrono_types::make_shared<SynSedanAgent>(i);
        agent_list[i] = agent;

        // Grid of vehicles
        int col = i % 3;
        int row = (i + 1) / 3;

        // Box dimensions
        double length = 400;
        double width = 25;

        ChVector<double> base = ChVector<double>({-length / 2 + 5, -width / 2 + 5, 1.0});
        ChVector<double> offset = ChVector<>({30.0 * row, 5.0 * col, 0});
        ChVector<double> initLoc = base + offset;

        ChQuaternion<> initRot = ChQuaternion<>({1, 0, 0, 0});
        agent->InitializeVehicle(ChCoordsys<>(initLoc, initRot));
        auto terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), 0, length, width);
        agent->SetTerrain(terrain);

        agent->SetRank(i);

        std::vector<ChVector<double>> path_points = {initLoc + ChVector<double>(0, 0, 0),
                                                     initLoc + ChVector<double>(length, 0, 0)};
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

        std::shared_ptr<ACCBrain> brain = chrono_types::make_shared<ACCBrain>(i, driver, agent->GetVehicle());
        agent->SetBrain(brain);

        std::shared_ptr<SynVisManager> vis_manager = chrono_types::make_shared<SynVisManager>();
        agent->AttachVisManager(vis_manager);

#ifdef IRRLICHT
        if (cli.HasValueInVector<int>("irr", i)) {
            std::shared_ptr<SynIrrVehicleVis> irr_vis = chrono_types::make_shared<SynIrrVehicleVis>(driver, STEP_SIZE);
            irr_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle());
            vis_manager->AttachVis(irr_vis);
        }
#endif

#ifdef SENSOR
        if (cli.HasValueInVector<int>("sens", i)) {
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

    double step = 0;
    int count = 0;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Simulation Loop
    while (step * HEARTBEAT < T_END) {
        std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

        // std::cout << count % num_vehs << std::endl;
        agent_list[count % num_vehs]->Advance(step * HEARTBEAT);

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        auto t = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        wall_time += t.count() / 1e6;

        if (verbose) {
            std::cout << "Completed timestep " << step << " in " << (t.count() / 1e6) << " seconds" << std::endl;
        }

        // increment the step
        step++;
        count++;

        if (verbose && fmod(step * HEARTBEAT, 1.0) <= 1e-5) {
            std::cout << "Wall Time " << wall_time << std::endl;
            wall_time = 0;
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // std::cout << "Total Wall Time: " << time_span.count() / 1e6 << "." << std::endl;

    // std::cout << "Fraction of real time: " << (time_span.count() / 1e6) / T_END << std::endl;
    std::cout << "real time: " << (time_span.count() / 1e6) / T_END << std::endl;

    return 0;
}
