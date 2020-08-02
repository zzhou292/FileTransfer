#include "synchrono/SynFramework.h"
#include "synchrono/SynVisManager.h"
#include "synchrono/SynMPIManager.h"
#include "synchrono/SynCLI.h"
#include "synchrono/SynVehicleBrain.h"
#include "synchrono/SynVehicleComponent.h"
#include "synchrono/brain/GeneralBrain.h"
#include "synchrono/component/TrackingComponent.h"

#include "synchrono/agent/SynCityBusAgent.h"
#include "synchrono/agent/SynSedanAgent.h"
#include "synchrono/agent/SynHMMWVAgent.h"
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

#include <chrono>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;

// =============================================================================


void A() 
{ 
    printf("This is callback function A invoked when rectangular detection is activated\n"); 
} 

void B()
{
    printf("This is callback function B invoked when circular detection is activated\n");
}


int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();
    cli.AddOption("Demo",
                  cxxopts::Option("v,vehicle", "Vehicle type", cxxopts::value<std::string>()->default_value("")));
    cli.Parse(argc, argv);
    cli.SetVerbosity(true);

    const double T_END = cli.GetAsType<double>("time_end");
    const double HEARTBEAT = cli.GetAsType<double>("heartbeat");
    const double STEP_SIZE = cli.GetAsType<double>("step_size");

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
        // Parse command line options to get vehicle type
        std::string vehicle_type = cli.GetAsType<std::string>("vehicle");
        std::shared_ptr<SynVehicleAgent> agent;
        if (vehicle_type.compare("Sedan") == 0) {
            agent = chrono_types::make_shared<SynSedanAgent>(rank);
        } else if (vehicle_type.compare("CityBus") == 0) {
            agent = chrono_types::make_shared<SynCityBusAgent>(rank);
        } else if (vehicle_type.compare("HMMWV") == 0) {
            agent = chrono_types::make_shared<SynHMMWVAgent>(rank);
        } else {
            std::cout << "Vehicle options: Sedan, CityBus, HMMWV.\nExitting..." << std::endl;
            exit(-1);
        }
        agent->SetStepSize(cli.GetAsType<double>("step_size"));
        mpi_manager.AddAgent(agent, rank);

        // Initialize the vehicle and the terrain
        agent->InitializeVehicle(ChCoordsys<>({0 + 4.0 * rank, 3.0 * rank, 0.5}, {1, 0, 0, 0}));
        auto terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), 0, 100, 100);
        agent->SetTerrain(terrain);

        for (int i = 1; i < num_ranks; i++) {
            if (i != rank) {
                std::shared_ptr<SynVehicleAgent> zombie_agent;
                if (vehicle_type.compare("Sedan") == 0) {
                    zombie_agent = chrono_types::make_shared<SynSedanAgent>(rank);
                } else if (vehicle_type.compare("CityBus") == 0) {
                    zombie_agent = chrono_types::make_shared<SynCityBusAgent>(rank);
                } else if (vehicle_type.compare("HMMWV") == 0) {
                    zombie_agent = chrono_types::make_shared<SynHMMWVAgent>(rank);
                }
                mpi_manager.AddAgent(zombie_agent, i);
            }
        }
        std::shared_ptr<ChDriver> driver = chrono_types::make_shared<ChDriver>(agent->GetVehicle());
        std::shared_ptr<GeneralBrain> brain =
            chrono_types::make_shared<GeneralBrain>(rank, driver, agent->GetVehicle());

        std::shared_ptr<TrackingComponent> module =
            chrono_types::make_shared<TrackingComponent>(rank, driver, agent->GetVehicle());

        module->enableLocDisplay();
        void (*ptr1)() = &A;
        module->addActionUsrRec(ptr1);

        // add TrackingComponent to the GeneralBrain
        brain->addModule(module);

        // TESTING BLOCK!!!!!!
        // return the amount of modules added to the brain
        std::cout << "module size : "<<brain->moduleVecLength() << std::endl;



        agent->SetBrain(brain);

        std::shared_ptr<SynVisManager> vis_manager = chrono_types::make_shared<SynVisManager>();
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
            std::string path = std::string("SENSOR_OUTPUT/Sedan") + std::to_string(rank) + std::string("/");

            std::shared_ptr<SynSenVehicleVis> sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
            sen_vis->InitializeDefaultSensorManager(agent->GetSystem());
            sen_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle().GetChassisBody());
            if (cli.GetAsType<bool>("sens_save"))
                sen_vis->AddFilterSave(path);
            if (cli.GetAsType<bool>("sens_vis"))
                sen_vis->AddFilterVisualize();
            vis_manager->AttachVis(sen_vis);
        }
#endif
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


  
