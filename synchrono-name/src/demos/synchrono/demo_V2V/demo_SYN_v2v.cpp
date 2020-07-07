#include "SynV2VBrain.h"

#include "synchrono/SynFramework.h"
#include "synchrono/SynVisManager.h"
#include "synchrono/SynMPIManager.h"
#include "synchrono/SynCLI.h"

#include "synchrono/agent/SynSedanAgent.h"
#include "synchrono/terrain/SynRigidTerrain.h"

#ifdef IRRLICHT
#include "synchrono/visualization/SynIrrVehicleVis.h"

#include "chrono_vehicle/driver/ChIrrGuiDriver.h"
#endif  // IRRLICHT

#ifdef SENSOR
#include "synchrono/visualization/SynSenVehicleVis.h"
#endif  // SENSOR

#include "chrono/core/ChVector.h"
#include "chrono_thirdparty/filesystem/path.h"
#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/ChGPSSensor.h"

#include <chrono>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;

// =============================================================================
const double T_END = 30.0;
const double HEARTBEAT = 0.01;
const double STEP_SIZE = 0.001;
const double SIM_EPSILON = 1e-9;

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();
    cli.Parse(argc, argv);

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

    if (rank != 0) {
        auto agent = chrono_types::make_shared<SynSedanAgent>(rank);
        mpi_manager.AddAgent(agent, rank);

        // Initialize the vehicle and the terrain
        agent->InitializeVehicle(ChCoordsys<>({0, 3.0 * rank, 0.5}, {1, 0, 0, 0}));
        auto terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), 0, 100, 100);
        agent->SetTerrain(terrain);

        // Create sensors to be used during inference
        // First create the sensor manager and setup the environment
        auto sensor_manager = chrono_types::make_shared<ChSensorManager>(agent->GetSystem());
        sensor_manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 5000);
        sensor_manager->scene->AddPointLight({-100, -100, 100}, {1, 1, 1}, 5000);

        // This agents GPS sensor
        int gps_update_rate = 100;
        float gps_lag = 1. / gps_update_rate;
        float gps_collection_window = 0;
        ChVector<> origin(-89.400, 43.070, 260.0);
        auto gps_noise_none = chrono_types::make_shared<ChGPSNoiseNone>();
        auto gps =
            chrono_types::make_shared<ChGPSSensor>(agent->GetVehicle().GetChassisBody(),  // body gps is attached to
                                                   gps_update_rate,                       // update rate in Hz
                                                   ChFrame<>({0, 0, 0}, {1, 0, 0, 0}),    // offset pose
                                                   gps_lag,                               // gps lag
                                                   gps_collection_window,                 // collection time
                                                   origin,                                // reference GPS location
                                                   gps_noise_none);                       // gps noise model
        gps->PushFilter(chrono_types::make_shared<ChFilterGPSAccess>());
        sensor_manager->AddSensor(gps);

        // This agents IMU sensor
        int imu_update_rate = 100;
        float imu_lag = 1. / imu_update_rate;
        float imu_collection_window = 0;
        auto imu_noise_none = chrono_types::make_shared<ChIMUNoiseNone>();
        auto imu =
            chrono_types::make_shared<ChIMUSensor>(agent->GetVehicle().GetChassisBody(),  // body imu is attached to
                                                   imu_update_rate,                       // update rate in Hz
                                                   ChFrame<>({0, 0, 0}, {1, 0, 0, 0}),    // offset pose
                                                   imu_lag,                               // imu lag
                                                   imu_collection_window,                 // collection time
                                                   imu_noise_none);                       // imu noise model
        imu->PushFilter(chrono_types::make_shared<ChFilterIMUAccess>());
        sensor_manager->AddSensor(imu);

        std::shared_ptr<ChDriver> driver = chrono_types::make_shared<ChDriver>(agent->GetVehicle());
        std::shared_ptr<SynV2VBrain> brain =
            chrono_types::make_shared<SynV2VBrain>(rank, driver, agent->GetVehicle(), gps, imu);
        agent->SetBrain(brain);

        // Create visualization manager which will faciliate sensor updates and irrlicht visualization (if needed)
        auto vis_manager = chrono_types::make_shared<SynVisManager>();
        // Attach sensor manager to visualization manager so that it gets updated during the simulation
        auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
        sen_vis->SetSensorManager(sensor_manager);
        vis_manager->AttachVis(sen_vis);
        agent->AttachVisManager(vis_manager);

#ifdef IRRLICHT
        if (cli.HasValueInVector<int>("irr", rank)) {
            std::shared_ptr<SynIrrVehicleVis> irr_vis = chrono_types::make_shared<SynIrrVehicleVis>(driver, STEP_SIZE);
            irr_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle());
            vis_manager->AttachVis(irr_vis);

            // Set driver as ChIrrGuiDriver
            auto driver = chrono_types::make_shared<ChIrrGuiDriver>(*irr_vis->GetIrrApp());
            auto brain = chrono_types::make_shared<SynVehicleBrain>(rank, driver, agent->GetVehicle());
            agent->SetBrain(brain);

            // Reset driver in visualization manager
            // TODO find a better solution to this
            irr_vis->SetDriver(driver);
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

        for (int i = 0; i < num_ranks; i++) {
            if (i != rank) {
                auto zombie_agent = chrono_types::make_shared<SynSedanAgent>(i, agent->GetSystem());
                zombie_agent->SetBrain(brain);
                mpi_manager.AddAgent(zombie_agent, i);
            }
        }
    }

    mpi_manager.Barrier();
    mpi_manager.Initialize();
    mpi_manager.Barrier();

    double step = 0;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Simulation Loop
    while (step * HEARTBEAT < T_END) {
        mpi_manager.Advance(step * HEARTBEAT);
        mpi_manager.Synchronize();
        mpi_manager.Broadcast();
        mpi_manager.Update();

        // increment the step
        step++;
    }

    std::cout << "Rank " << rank << " completed successfully" << std::endl;

    return 0;
}
