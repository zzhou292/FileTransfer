#include "chrono/core/ChStream.h"
    make_sharedchrono_types::make_sharedchrono_types::make_sharedchrono_types::make_sharedchrono_types::
        make_sharedchrono_types::make_sharedchrono_types::make_sharedchrono_types::make_sharedchrono_types::make_shared
#include "chrono/core/ChRealtimeStep.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChLinkMate.h"
#include "chrono/assets/ChTexture.h"
#include "chrono/assets/ChColorAsset.h"
#include "chrono_irrlicht/ChIrrApp.h"
#include "chrono/core/ChQuaternion.h"

#include "chrono_thirdparty/filesystem/path.h"
// MPI include
#include <mpi.h>
// #include "synchrono_mpi/MPISync.h"
#include <chrono>
#include <map>
#include <iostream>

#include "synchrono_agent/SynAgent.h"
#include "synchrono_agent/SedanAgent.h"
#include "synchrono_agent/TrafficLightAgent.h"
#include "synchrono_agent/SynFramework.h"

#include "synchrono_interface/SynInterface.h"
#include "synchrono_interface/SynInterfaceDriver.h"

#include "SynCameraComponent.h"
#include "SynTimeComponent.h"
#include "SynControlComponent.h"
// Synchrono include
// #include "demos/synchrono/syn-helpers.h"

using namespace synchrono::agent;
using namespace synchrono::interface;

using namespace chrono;
using namespace chrono::irrlicht;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;
using namespace irr::gui;

// =============================================================================
const double T_END = 100.0;
const double HEARTBEAT = 0.01;
const double STEP_SIZE = 0.001;
const double SIM_EPSILON = 1e-9;

const double BCAST_RADIUS = 5;

// =============================================================================

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage :: " << std::string(argv[0]) << " <port number>" << std::endl;
        return 1;
    }
    std::string port = std::string(argv[1]);
    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);

    // ------------------
    // MPI Initialization
    // ------------------

    std::map<int, std::shared_ptr<SynAgent>> agent_list;

    MPI_Init(&argc, &argv);

    int myRank, numRanks;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

    std::vector<double> agent_state;
    int state_length = 36;
    double wall_time = 0;

    int msg_length = 90;
    std::vector<double> agent_msg = std::vector<double>(msg_length);

    std::vector<double> all_states = std::vector<double>(state_length * numRanks);
    std::vector<double> all_msg = std::vector<double>(msg_length * numRanks);

    // ======================
    // intialization phase
    // ======================

    // Add agents to the agent list, same agent_list across all ranks
    // for (int i = 1; i < numRanks; i++) {
    //	// JHT - 1, 1 and 0.3 are some standard values Asher had...
    //       /*HumanDriver *brain = new HumanDriver(i, 1.0, 1.0, 0.3);*/
    //       auto agent = chrono_types::make_shared<SedanAgent>(i/*, brain*/);
    //       agent_list[i] = agent;
    //}

    // int temp;
    // if (myRank == 0) {
    //       std::cout << "Please input number: ";
    //       std::cin >> temp;
    //   }
    std::shared_ptr<SynAgent> ego_agent;
    std::shared_ptr<SynInterface> interface;
    if (myRank == 1) {
        // can test with 1, 2, 3
        int testTrafficLightInstance = 1;  // in final demo, rank 1, 2, 3 each has trafficLight1, 2, 3

        auto ego_agent = chrono_types::make_shared<Traffic_light>(testTrafficLightInstance);

        ego_agent->Initialize({0, 2.0 * myRank, 0.5}, {1, 0, 0, 0}, "", "");

        int numberOfLanes;
        if (testTrafficLightInstance == 1) {
            numberOfLanes = 8;
        }
        if (testTrafficLightInstance == 2) {
            numberOfLanes = 7;
        }
        if (testTrafficLightInstance == 3) {
            numberOfLanes = 4;
        }

        for (int i = 0; i < numberOfLanes; ++i) {
            // ego_agent->addLane(ChVector<>(i + 0.5, i + 1.5, i + 2.5), ChVector<>(i + 3.5, i + 4.5, i + 5.5), 2.5);
        }
        agent_list[myRank] = ego_agent;
        ego_agent->SecondIrrInit();
    } else if (myRank > 1) {
        // Set up our specific (SedanAgent & IrrBrain) agent
        auto ego_agent = chrono_types::make_shared<SedanAgent>(myRank);
        agent_list[myRank] = ego_agent;

        // Relative path likely only good for Windows
        // ego_agent->Initialize({0, 2.0 * myRank, 0.5}, {1, 0, 0, 0},
        //                       "../../../libraries/assets/park_street_col_2_rot.obj");
        // ego_agent->Initialize({-50 + 9.0 * myRank, 0, 0.5}, {1, 0, 0, 0}, "");

        ChVector<> initLoc;
        ChQuaternion<> initRot;
        int rankGroup = (myRank + 1) % 3;
        std::cout << "Rank Group :: " << rankGroup << std::endl;
        switch (rankGroup) {
            case 0:
                initLoc = ChVector<>({75.0 - (myRank - 2) / 3 * 9, -7.5, 0.5});
                initRot = ChQuaternion<>({-0.00942852, 0.00497593, -0.00215422, -0.999941});
                break;
            case 1:
                initLoc = ChVector<>({2, -40.0 - (myRank - 3) / 3 * 9, 0.5});
                initRot = ChQuaternion<>({-0.7071068, 0, 0, -0.7071068});
                break;
            case 2:
                initLoc = ChVector<>({-3, 65.0 + (myRank - 4) / 3 * 9, 0.5});
                initRot = ChQuaternion<>({-0.7071068, 0, 0, 0.7071068});
                break;
            default:
                initLoc = ChVector<>({75.0 - (myRank - 1) * 9, -7.5, 0.5});
                initRot = ChQuaternion<>({-0.00942852, 0.00497593, -0.00215422, -0.999941});
                break;
        }
        // initRot({-0.00942852, 0.00497593, -0.00215422, -0.999941});
        // ChQuaternion<> initRot({0.7071068, 0, 0, -0.7071068});
        // ChVector<> initLoc({2, -40, 0.5});
        // ChVector<> initLoc({-8, 40, 0.5});

        // Relative path
        ego_agent->Initialize(initLoc, initRot, "/home/aaron/Continental/MeshExport/park_street_col_2.obj",
                              "/home/aaron/Continental/MeshExport/park_street_vis_split.obj");
        // ego_agent->Initialize(initLoc, initRot, "../../../libraries/assets/park_street_col_2.obj",
        // "../../../libraries/assets/park_street_vis_split.obj");

        // std::vector<ChVector<>> curve_pts = {	ChVector<>(0, 2, 0),
        //										ChVector<>(3, 2, 0),
        //										ChVector<>(5, 5, 0),
        //										ChVector<>(5, 10, 0),
        //										ChVector<>(0, 20, 0),
        //										ChVector<>(-5, 5, 0)};
        // std::shared_ptr<ChBezierCurve> path = chrono_types::make_shared<ChBezierCurve>(curve_pts);
        // ego_agent->InitCurveFollowBrain(ego_agent->GetVehicle(), path, "Jay's path", 10, 1, 10, 100, 1, 1, 1, true);

        //// Stuff for GPS paths on Park st.
        std::vector<GPScoord> gps_john_inner = {
            GPScoord(43.0732523, -89.4000448), GPScoord(43.0732582, -89.4003855), GPScoord(43.0732680, -89.4011392),
            GPScoord(43.0732758, -89.4015884), GPScoord(43.0732111, -89.4017427), GPScoord(43.0728594, -89.4018459),
            GPScoord(43.0724460, -89.4018593), GPScoord(43.0722236, -89.4017963), GPScoord(43.0721296, -89.4016863),
            GPScoord(43.0721227, -89.4013967), GPScoord(43.0721169, -89.4010533), GPScoord(43.0721080, -89.4003707),
            GPScoord(43.0721022, -89.3999308), GPScoord(43.0722697, -89.3996707), GPScoord(43.0725753, -89.3996063),
            GPScoord(43.0729280, -89.3996170), GPScoord(43.0731749, -89.3997324), GPScoord(43.0732366, -89.3998651),
            GPScoord(43.0732503, -89.4000033)};
        std::vector<GPScoord> gps_grainger_loop = {
            GPScoord(43.0721590, -89.4016810), GPScoord(43.0721541, -89.4015013), GPScoord(43.0721531, -89.4013175),
            GPScoord(43.0721492, -89.4011378), GPScoord(43.0721467, -89.4010379), GPScoord(43.0721448, -89.4008421),
            GPScoord(43.0721683, -89.4007080), GPScoord(43.0722452, -89.4006731), GPScoord(43.0723858, -89.4006396),
            GPScoord(43.0725440, -89.4006081), GPScoord(43.0727282, -89.4005920), GPScoord(43.0728869, -89.4005987),
            GPScoord(43.0730466, -89.4006054), GPScoord(43.0731181, -89.4006041), GPScoord(43.0732131, -89.4006470),
            GPScoord(43.0732337, -89.4008213), GPScoord(43.0732395, -89.4011593), GPScoord(43.0732454, -89.4014892),
            GPScoord(43.0731984, -89.4017145), GPScoord(43.0730005, -89.4018003), GPScoord(43.0727928, -89.4018057),
            GPScoord(43.0725087, -89.4018111), GPScoord(43.0724362, -89.4018137), GPScoord(43.0723774, -89.4018084),
            GPScoord(43.0723069, -89.4017923), GPScoord(43.0722266, -89.4017574), GPScoord(43.0721952, -89.4017306),
            GPScoord(43.0721727, -89.4017051)};
        std::vector<GPScoord> gps_park_straight = {
            GPScoord(43.0738876, -89.4006684), GPScoord(43.0737201, -89.4006705), GPScoord(43.0736167, -89.4006705),
            GPScoord(43.0735016, -89.4006725), GPScoord(43.0733346, -89.4006899), GPScoord(43.0731700, -89.4007207),
            GPScoord(43.0730083, -89.4007315), GPScoord(43.0728903, -89.4007342), GPScoord(43.0727331, -89.4007449),
            GPScoord(43.0725910, -89.4007489), GPScoord(43.0724881, -89.4007623), GPScoord(43.0722883, -89.4008106),
            GPScoord(43.0720267, -89.4008803), GPScoord(43.0718778, -89.4009206), GPScoord(43.0716535, -89.4009501),
            GPScoord(43.0714614, -89.4009514), GPScoord(43.0712733, -89.4009554), GPScoord(43.0711205, -89.4009635),
            GPScoord(43.0708217, -89.4009702), GPScoord(43.0704739, -89.4009782), GPScoord(43.0702506, -89.4009890),
            GPScoord(43.0700007, -89.4010064), GPScoord(43.0699351, -89.4009085), GPScoord(43.0699929, -89.4007905),
            GPScoord(43.0701849, -89.4007757), GPScoord(43.0704328, -89.4007690), GPScoord(43.0706983, -89.4007610),
            GPScoord(43.0708393, -89.4007549), GPScoord(43.0710921, -89.4007563), GPScoord(43.0713155, -89.4007516),
            GPScoord(43.0715364, -89.4007409), GPScoord(43.0718582, -89.4007207), GPScoord(43.0719577, -89.4007060),
            GPScoord(43.0722310, -89.4006403), GPScoord(43.0724303, -89.4005860), GPScoord(43.0726674, -89.4005551),
            GPScoord(43.0727776, -89.4005464), GPScoord(43.0729736, -89.4005370), GPScoord(43.0730960, -89.4005316),
            GPScoord(43.0732645, -89.4005296), GPScoord(43.0734365, -89.4005779), GPScoord(43.0735873, -89.4006081),
            GPScoord(43.0737646, -89.4006249), GPScoord(43.0738988, -89.4006275), GPScoord(43.0740923, -89.4006242),
            GPScoord(43.0745415, -89.4006188), GPScoord(43.0747110, -89.4006161), GPScoord(43.0750979, -89.4006027),
            GPScoord(43.0753203, -89.4004405), GPScoord(43.0755750, -89.4004083), GPScoord(43.0757562, -89.4005062),
            GPScoord(43.0757611, -89.4007301), GPScoord(43.0755916, -89.4008173), GPScoord(43.0753516, -89.4007972),
            GPScoord(43.0750950, -89.4006631), GPScoord(43.0747159, -89.4006577), GPScoord(43.0743896, -89.4006591),
            GPScoord(43.0741369, -89.4006617), GPScoord(43.0739870, -89.4006658)};

        std::shared_ptr<SynFramework> framework =
            chrono_types::make_shared<SynFramework>(GPScoord(43.073268, -89.400636), ego_agent->getTerrain());

        // Make paths from the GPS points
        std::shared_ptr<ChBezierCurve> path_john_inner = framework->getCurveFromGPS(gps_john_inner, 0, true);
        std::shared_ptr<ChBezierCurve> path_grainger_loop = framework->getCurveFromGPS(gps_grainger_loop, 0, true);
        std::shared_ptr<ChBezierCurve> path_park_straight = framework->getCurveFromGPS(gps_park_straight, 0, true);

        std::shared_ptr<ChBezierCurve> path;
        switch (rankGroup) {
            case 0:
                path = path_john_inner;
                break;
            case 1:
                path = path_grainger_loop;
                break;
            case 2:
                path = path_park_straight;
                break;
            default:
                path = path_john_inner;
                break;
        }
        // Set up the zombie agents, do need to know their type (SedanAgent) but not brain
        for (int i = 1; i < numRanks; i++) {
            if (i != myRank) {
                if (i == 1) {
                    agent_list[i] = chrono_types::make_shared<Traffic_light>(3);
                    agent_list[i]->Initialize({0, 2.0 * myRank, 0.5}, {1, 0, 0, 0}, "", "");
                } else {
                    agent_list[i] = chrono_types::make_shared<SedanAgent>(i);
                }
                agent_list[i]->InitializeZombie(ego_agent->GetSystem());
            }
        }

#ifdef IRRLICHT
        ego_agent->SecondIrrInit();
#endif
#ifdef SENSOR
        ego_agent->InitSensors();
#endif

        if (myRank == 2) {
            // Set the time response for steering and throttle keyboard inputs.
            double steering_time = 1.0;  // time to go from 0 to +1 (or from 0 to -1)
            double throttle_time = 1.0;  // time to go from 0 to +1
            double braking_time = 0.3;   // time to go from 0 to +1

            std::shared_ptr<SynInterfaceDriver> driver = chrono_types::make_shared<SynInterfaceDriver>(ego_agent->GetVehicle());

            driver->SetSteeringDelta(STEP_SIZE / steering_time);
            driver->SetThrottleDelta(STEP_SIZE / throttle_time);
            driver->SetBrakingDelta(STEP_SIZE / braking_time);
            driver->Initialize();
            ego_agent->SetDriver(std::static_pointer_cast<ChDriver>(driver));

            // Create the interface
            interface = chrono_types::make_shared<SynInterface>(port);

            try {
#ifdef SENSOR
                interface->Add(chrono_types::make_shared<SynCameraComponent>(ego_agent->GetCamera()->GetName(),
                                                                    ego_agent->GetCamera()->GetUpdateRate(),
                                                                    ego_agent->GetCamera()));
#endif
                interface->Add(chrono_types::make_shared<SynTimeComponent>("Time", ego_agent->GetSystem()));
                interface->Add(chrono_types::make_shared<SynControlComponent>("Control", *driver));
                // interface.Add(chrono_types::make_shared<SynLidarComponent>(lidar->GetName(), lidar->GetUpdateRate(), lidar));
            } catch (const char* err) {
                std::cerr << err << std::endl;
                return -1;
            }
        } else {
            ego_agent->InitCurveFollowBrain(ego_agent->GetVehicle(), path, "Jay's path", 10, 1, 10, 100, .8, 0, 0,
                                            true);
        }
    }

    double sim_time = 0;
    int step = 1;

    std::cout << std::endl;
    // everyone should be on the same page before we progress
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Passed barrier" << std::endl;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::cout << myRank << std::endl;
    // simulation loop
    while (step * HEARTBEAT < T_END) {
        // if (myRank == 0) {
        // std::cout << "Time: " << step * HEARTBEAT << std::endl;
        // }
        std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

        // collect states from the agent we are responsible for
        if (myRank != 0) {
            agent_state = agent_list[myRank]->GetState();
            // std::cout << "Time for agent: " << myRank << ": " << agent_state[0] << std::endl;
            agent_list[myRank]->GetMessages(step * HEARTBEAT);

            // iter for message offset
            int iter = 0;
            while (!agent_list[myRank]->out_messages.empty()) {
                auto msg = agent_list[myRank]->out_messages.front();

                std::shared_ptr<synchrono::MessageMAP> msgMap;
                std::shared_ptr<synchrono::MessageSPAT> msgSpat;
                switch (msg->type) {
                    case synchrono::Message::MAP:
                        msgMap = std::static_pointer_cast<synchrono::MessageMAP>(msg);
                        agent_msg[iter] = 0;
                        agent_msg[iter + 1] = (double)msgMap->senderID;
                        agent_msg[iter + 2] = msgMap->time;
                        agent_msg[iter + 3] = (double)msgMap->intersectionID;
                        agent_msg[iter + 4] = (double)msgMap->num_lanes;
                        iter += 5;
                        for (int i = 0; i < msgMap->num_lanes; ++i) {
                            agent_msg[iter + i] = msgMap->widths[i];
                        }
                        iter += msgMap->num_lanes;
                        for (int i = 0; i < msgMap->num_lanes; ++i) {
                            agent_msg[iter + i * 3] = msgMap->stopPoints[i].x();
                            agent_msg[iter + i * 3 + 1] = msgMap->stopPoints[i].y();
                            agent_msg[iter + i * 3 + 2] = msgMap->stopPoints[i].z();
                        }
                        iter += msgMap->num_lanes * 3;
                        for (int i = 0; i < msgMap->num_lanes; ++i) {
                            agent_msg[iter + i * 3] = msgMap->offsetPoints[i].x();
                            agent_msg[iter + i * 3 + 1] = msgMap->offsetPoints[i].y();
                            agent_msg[iter + i * 3 + 2] = msgMap->offsetPoints[i].z();
                        }
                        iter += msgMap->num_lanes * 3;
                        break;
                    case synchrono::Message::SPAT:
                        msgSpat = std::static_pointer_cast<synchrono::MessageSPAT>(msg);
                        agent_msg[iter] = 1;
                        agent_msg[iter + 1] = (double)msgSpat->senderID;
                        agent_msg[iter + 2] = msgSpat->time;
                        agent_msg[iter + 3] = (double)msgSpat->intersectionID;
                        agent_msg[iter + 4] = (double)msgSpat->num_lanes;
                        iter += 5;

                        for (int i = 0; i < msgSpat->num_lanes; ++i) {
                            agent_msg[iter + i] = (double)msgSpat->colors[i];
                        }
                        iter += msgSpat->num_lanes;
                        break;
                    case synchrono::Message::VEH:
                        agent_msg = {2,
                                     (double)std::static_pointer_cast<synchrono::MessageVEH>(msg)->senderID,
                                     (double)std::static_pointer_cast<synchrono::MessageVEH>(msg)->time,
                                     std::static_pointer_cast<synchrono::MessageVEH>(msg)->location.x(),
                                     std::static_pointer_cast<synchrono::MessageVEH>(msg)->location.y(),
                                     std::static_pointer_cast<synchrono::MessageVEH>(msg)->location.z()};

                        break;
                    default:
                        break;
                }
                agent_list[myRank]->out_messages.pop();
            }
        }

        // synchronize between ranks
        if (myRank == 0) {
            double max_sync_error = 0;
            for (int i = 1; i < numRanks; i++) {
                MPI_Recv(&all_states.data()[i * state_length], state_length, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                MPI_Recv(&all_msg.data()[i * msg_length], msg_length, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
                // if (abs(all_states.data()[i * state_length] - (step - 1) * HEARTBEAT) > max_sync_error) {
                //     max_sync_error = abs(all_states.data()[i * state_length] - (step - 1) * HEARTBEAT);
                // }
            }

            // std::cout << "Total Synchronization Error: " << max_sync_error << std::endl;

        } else {
            MPI_Send(agent_state.data(), state_length, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            MPI_Send(agent_msg.data(), msg_length, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

        // send the entire set of collected states to all the ranks
        MPI_Bcast(all_states.data(), numRanks * state_length, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(all_msg.data(), numRanks * msg_length, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // update the zombie agents
        if (myRank > 1) {
            for (int i = 2; i < numRanks; i++) {
                if (i != myRank) {
                    std::vector<double>::const_iterator z_start = all_states.begin() + i * state_length;
                    std::vector<double>::const_iterator z_end = all_states.begin() + (i + 1) * state_length;
                    std::vector<double> zState(z_start, z_end);

                    // advance the agent to a specific time where we will synchronize
                    agent_list[i]->SynchronizeZombie(zState);
                }
            }
        }

        if (myRank > 1) {
            for (int i = 1; i < numRanks; ++i) {
                if (i != myRank) {
                    if ((unsigned int)(all_msg.data()[i * msg_length] + 0.5) == 0) {
                        int iter = 0;
                        std::shared_ptr<synchrono::MessageMAP> new_map_msg =
                            chrono_types::make_shared<synchrono::MessageMAP>();
                        new_map_msg->type = synchrono::Message::MAP;
                        new_map_msg->senderID = (unsigned int)(all_msg.data()[i * msg_length + 1] + 0.5);
                        new_map_msg->time = (float)(all_msg.data()[i * msg_length + 2]);
                        new_map_msg->intersectionID = (unsigned int)(all_msg.data()[i * msg_length + 3] + 0.5);
                        new_map_msg->num_lanes = (short int)(all_msg.data()[i * msg_length + 4] + 0.5);

                        iter += 5;
                        for (int j = 0; j < new_map_msg->num_lanes; ++j) {
                            new_map_msg->widths.push_back(all_msg.data()[i * msg_length + iter + j]);
                        }
                        iter += new_map_msg->num_lanes;
                        for (int j = 0; j < new_map_msg->num_lanes; ++j) {
                            new_map_msg->stopPoints.push_back(
                                ChVector<>(all_msg.data()[i * msg_length + iter + 3 * j],
                                           all_msg.data()[i * msg_length + iter + 3 * j + 1],
                                           all_msg.data()[i * msg_length + iter + 3 * j + 2]));
                        }
                        iter += new_map_msg->num_lanes * 3;
                        for (int j = 0; j < new_map_msg->num_lanes; ++j) {
                            new_map_msg->offsetPoints.push_back(
                                ChVector<>(all_msg.data()[i * msg_length + iter + 3 * j],
                                           all_msg.data()[i * msg_length + iter + 3 * j + 1],
                                           all_msg.data()[i * msg_length + iter + 3 * j + 2]));
                        }
                        iter += new_map_msg->num_lanes * 3;

                        std::shared_ptr<synchrono::MessageSPAT> new_spat_msg =
                            chrono_types::make_shared<synchrono::MessageSPAT>();
                        new_spat_msg->type = synchrono::Message::SPAT;
                        new_spat_msg->senderID = (unsigned int)(all_msg.data()[i * msg_length + iter + 1] + 0.5);
                        new_spat_msg->time = (float)(all_msg.data()[i * msg_length + iter + 2]);
                        new_spat_msg->intersectionID = (short int)(all_msg.data()[i * msg_length + iter + 3] + 0.5);
                        new_spat_msg->num_lanes = (float)(all_msg.data()[i * msg_length + iter + 4]);

                        iter += 5;

                        for (int j = 0; j < new_spat_msg->num_lanes; ++j) {
                            new_spat_msg->colors.push_back(
                                (synchrono::Message::Color)((int)(all_msg.data()[i * msg_length + iter + j] + 0.5)));
                        }

                        agent_list[myRank]->m_messages.push(new_map_msg);
                        agent_list[myRank]->m_messages.push(new_spat_msg);
                    } else {  // message to receive must be sent from a vehicle in this case
                        if ((ChVector<>(all_states.data()[i * state_length + 1],
                                        all_states.data()[i * state_length + 2],
                                        all_states.data()[i * state_length + 3]) -
                             ChVector<>(agent_state.data()[1], agent_state.data()[2], agent_state.data()[3]))
                                .Length() <= BCAST_RADIUS) {
                            std::shared_ptr<synchrono::MessageVEH> new_msg =
                                chrono_types::make_shared<synchrono::MessageVEH>();
                            new_msg->type = synchrono::Message::VEH;
                            new_msg->senderID = (unsigned int)(all_msg.data()[i * msg_length + 1] + 0.5);
                            new_msg->time = (float)(all_msg.data()[i * msg_length + 2]);
                            new_msg->location =
                                ChVector<>(all_msg.data()[i * msg_length + 3], all_msg.data()[i * msg_length + 4],
                                           all_msg.data()[i * msg_length + 5]);
                            agent_list[myRank]->m_messages.push(new_msg);
                        }
                    }
                }
            }
        }

        // // advance the live agent
        if (myRank != 0) {
            // advance the agent to a specific time where we will synchronize
            if (myRank == 2)
                interface->Synchronize(step * HEARTBEAT);
            agent_list[myRank]->Advance(step * HEARTBEAT);
        }

        // increment the step
        step++;

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> t = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
        wall_time += t.count();

        if (fmod(step * HEARTBEAT, 1.0) <= 1e-5 && myRank == 3) {
            std::cout << "Wall Time :: " << wall_time << std::endl;
            wall_time = 0;
        }
    }

    if (myRank == 0) {
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Wall Time: " << time_span.count() << ".\n";
    }

    // should eventually look up what sort of agent we are from an initialization file of some sort
    // but for now we are all sedans

    MPI_Finalize();

    return 0;
}
