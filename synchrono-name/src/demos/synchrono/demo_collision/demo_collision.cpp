#include "synchrono/SynMPIManager.h"
#include "synchrono/SynFramework.h"
#include "synchrono/SynVisManager.h"
#include "synchrono/SynCLI.h"

#include "synchrono/brain/TBrain.h"
#include "synchrono/brain/ACCBrain.h"
#include "synchrono/brain/TrafficLightBrain.h"
#include "synchrono/agent/SynSedanAgent.h"
#include "synchrono/agent/SynHMMWVAgent.h"
#include "synchrono/agent/SynFullHMMWVAgent.h"
#include "synchrono/agent/SynTrafficLightAgent.h"
#include "synchrono/agent/SynCityBusAgent.h"

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
const double HEARTBEAT = 0.01;
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

    const double T_END = 27;
    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    // For timing
    double wall_time = 0;

    // ====================
    // Agent Initialization
    // ====================
    SynMPIManager mpi_manager(argc, argv);


    int rank = mpi_manager.GetRank();
    int num_ranks = mpi_manager.GetNumRanks();
    std::shared_ptr<ChPathFollowerACCDriver> driver; 

    std::shared_ptr<ChBezierCurve> path_1 = NULL;
    std::shared_ptr<ChBezierCurve> path_2 = NULL;
    std::shared_ptr<ChBezierCurve> path_3 = NULL;

    chrono::Vector resultVec1;
    chrono::Vector resultVec2;

    if (rank != 0) {
        // -----------------------------------
        // Sedan Agents (Rank: 1 to num_ranks)
        // -----------------------------------
        for (int i = 1; i < num_ranks; i++) {
            std::shared_ptr<SynVehicleAgent> agent = NULL;
            if(i==1)
            {
                agent = chrono_types::make_shared<SynSedanAgent>(i);
                mpi_manager.AddAgent(agent, i);
            }
            else if(i==2)
            {
                agent = chrono_types::make_shared<SynSedanAgent>(i);
                mpi_manager.AddAgent(agent, i);
            }

            ChVector<> initLoc;
            ChVector<> initLoc2;
            ChQuaternion<> initRot;

            // rank = 3*rank - 2;
            ChQuaternion<> qA_1;
            ChQuaternion<> qB_1;
            std::cout<<"rank: "<<rank<<std::endl; 
            switch (rank) {
                case 1:
                    // Sedan Loop
                    initLoc = ChVector<>({-2.8, -70, 0.72});

                    initRot = QUNIT;
                    qA_1 = Q_from_AngAxis(0 * CH_C_DEG_TO_RAD, VECT_Y);
                    qB_1 = Q_from_AngAxis(90 * CH_C_DEG_TO_RAD, VECT_Z);
                    initRot = initRot >> qA_1 >> qB_1;
                    break;
                case 2:
                    // City bus
                    initLoc = ChVector<>({-6.4, -70, 0.72});
                    initLoc2 = ChVector<>({-2.8, -70, 0.72});
                    initRot = QUNIT;
                    qA_1 = Q_from_AngAxis(0 * CH_C_DEG_TO_RAD, VECT_Y);
                    qB_1 = Q_from_AngAxis(90 * CH_C_DEG_TO_RAD, VECT_Z);
                    initRot = initRot >> qA_1 >> qB_1;
                    break;
                default:
                    std::cerr << "Unexpectedly reached default case statement" << std::endl;
                    // University facing west, just before Park. Second lane from the left.
                    break;
            }


            if (i == rank) {
                agent->InitializeVehicle(ChCoordsys<>(initLoc, initRot));
                auto terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(),
                "/home/jz/Desktop/chrono-test/GreatJason/Highway_col.obj","/home/jz/Desktop/chrono-test/GreatJason/Highway.obj");
                agent->SetTerrain(terrain);

                agent->SetRank(rank);

                std::vector<ChVector<>> gps_group_1;
                gps_group_1 = {initLoc, 
                initLoc + ChVector<>(0.00,160.00,0.00)};
                std::vector<ChVector<>> gps_group_2;
                gps_group_2 = {initLoc, 
                initLoc + ChVector<>(0.00,160.00,0.00)};
                std::vector<ChVector<>> gps_group_3;
                gps_group_3 = {initLoc2, 
                initLoc2 + ChVector<>(0.00,160.00,0.00)};

                // Make paths from the GPS points
                path_1 = chrono_types::make_shared<ChBezierCurve>(gps_group_1);
                path_2 = chrono_types::make_shared<ChBezierCurve>(gps_group_2);
                path_3 = chrono_types::make_shared<ChBezierCurve>(gps_group_3);

                std::shared_ptr<ChBezierCurve> path;
                int lane;
                switch (rank) {
                    case 1:
                        path = path_1;
                        lane = 1;
                        break;
                    case 2:
                        path = path_2;
                        lane = 2;
                    default:
                        path = path_1;
                        break;
                }

                double target_speed;
                std::cout<< "wall time: "<<wall_time<<std::endl;
                switch (rank) {
                    case 1:
                        target_speed = 8;
                        break;
                    case 2:
                        target_speed = 8;
                        break;
                    default:
                        target_speed = 8;
                        break;
                }

                double target_following_time = 1.2;
                double target_min_distance = 10;
                double current_distance = 100;
                bool isPathClosed = false;

                driver = chrono_types::make_shared<ChPathFollowerACCDriver>(
                    agent->GetVehicle(), path, "Park St", target_speed, target_following_time, target_min_distance,
                    current_distance, isPathClosed);

                double speed_Kp = 0.2;
                double speed_Ki = 0;
                double speed_Kd = 0;

                double turn_Kp = 0.5;
                double turn_Ki = 0.1;
                double turn_Kd = 0.2;
                driver->GetSpeedController().SetGains(speed_Kp, speed_Ki, speed_Kd);
                driver->GetSteeringController().SetGains(turn_Kp, turn_Ki, turn_Kd);
                driver->GetSteeringController().SetLookAheadDistance(5);

                std::shared_ptr<TBrain> brain =
                    chrono_types::make_shared<TBrain>(rank, driver, agent->GetVehicle());
                    //-------------------------------
                std::cout<<&brain<<std::endl;
                brain->setLane(lane);
                agent->SetBrain(brain);

                //std::shared_ptr<TBrain> Tbrain =
                //    chrono_types::make_shared<TBrain>(rank, driver, agent->GetVehicle());

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
                    auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>(SynSenVehicleVis::CAMERA);

                    auto manager = chrono_types::make_shared<ChSensorManager>(agent->GetSystem());
                    manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 2000);
                    manager->scene->AddPointLight({-100, 100, 100}, {1, 1, 1}, 2000);
                    sen_vis->SetSensorManager(manager);

                    auto origin = chrono_types::make_shared<ChBody>();
                    origin->SetBodyFixed(true);
                    agent->GetSystem()->AddBody(origin);

                    ChVector<> camera_loc(0, 0, 45);

                    // Rotations to get a nice angle

               
                    ChQuaternion<> rotation = QUNIT;
                    ChQuaternion<> qA = Q_from_AngAxis(90 * CH_C_DEG_TO_RAD, VECT_Y);
                    ChQuaternion<> qB = Q_from_AngAxis(180 * CH_C_DEG_TO_RAD, VECT_Z);
                    rotation = rotation >> qA >> qB;
                    
                    
                    double cam_res_width = 1280;
                    double cam_res_height = 720;

                    auto intersection_camera = chrono_types::make_shared<chrono::sensor::ChCameraSensor>(
                        origin,                                         // body camera is attached to
                        30,                                             // update rate in Hz
                        chrono::ChFrame<double>(camera_loc, rotation),  // offset pose
                        cam_res_width,                                  // image width
                        cam_res_height,                                 // image height
                        CH_C_PI / 3, 1. / 30., 0);

                    intersection_camera->SetName("Intersection Cam");
                    intersection_camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());
                    if (cli.GetAsType<bool>("sens_vis"))
                        intersection_camera->PushFilter(
                            chrono_types::make_shared<ChFilterVisualize>(cam_res_width, cam_res_height));

                    std::string path = std::string("SENSOR_OUTPUT/Sedan") + std::to_string(rank) + std::string("/");
                    if (cli.GetAsType<bool>("sens_save")) {
                        intersection_camera->PushFilter(chrono_types::make_shared<ChFilterSave>(path));
                    }

                    sen_vis->SetSensor(intersection_camera);
                    vis_manager->AttachVis(sen_vis);
                }
#endif  // SENSOR
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
        
        if(rank == 1)
        {
            resultVec1 = driver->GetVEHSentinel();
            
            mpi_manager.Track(resultVec1);
            //std::cout<<"Location = "<<resultVec1<<std::endl;
            //std::cout<<"Location 2 = " << resultVec2<<std::endl;
        }
        else if(rank == 2)
        {
            resultVec2 = driver->GetVEHSentinel();
            
            mpi_manager.Track(resultVec2);
            //std::cout<<"Location 2 = " << resultVec2<<std::endl;
        }
     //std::cout<<"MPI STORED Location - " << 1 << " - "<< mpi_manager.ReturnTrackingData(1)<<std::endl;
        //std::cout<<"MPI STORED Location - " << 2 << " - "<< mpi_manager.ReturnTrackingData(2)<<std::endl;
        //std::cout<<"MPI STORED Location 2 = " << mpi_manager.ReturnTrackingData(2)<<std::endl;

        
        
        if(rank==2 && (step * HEARTBEAT == 5))
        {
            driver->ChangeVEHPath(path_3, false);
        }

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

        if (fmod(step * HEARTBEAT, 1.0) <= 1e-5) {
            // std::cout << "Wall Time (" << rank << "):: " << wall_time << std::endl;
            wall_time = 0;
        }
    }

    if (rank == 0) {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // std::cout << "Total Wall Time: " << time_span.count() / 1e6 << "." << std::endl;

        std::cout << "Fraction of real time: " << (time_span.count() / 1e6) / T_END << std::endl;
    }

    return 0;
}
