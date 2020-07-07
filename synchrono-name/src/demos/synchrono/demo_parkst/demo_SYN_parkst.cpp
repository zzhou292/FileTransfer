#include "synchrono/SynMPIManager.h"
#include "synchrono/SynFramework.h"
#include "synchrono/SynVisManager.h"
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

std::cout<< "initial" << std::endl;

    const double T_END = cli.GetAsType<double>("time_end");

    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    // For timing
    double wall_time = 0;

    // ====================
    // Agent Initialization
    // ====================
    SynMPIManager mpi_manager(argc, argv);

std::cout<< "start flag" << std::endl;

    int rank = mpi_manager.GetRank();
    int num_ranks = mpi_manager.GetNumRanks();



    if (rank != 0) {
        // -----------------------------
        // Traffic Light Agent (Rank: 1)
        // -----------------------------
        // In final demo, rank 1, 2 and 3 will each handle a different intersection
        // For this demo, rank 1 is only used, which is the intersection of park st and University
        auto agent = chrono_types::make_shared<SynTrafficLightAgent>(1);
        mpi_manager.AddAgent(agent, 1);
        std::shared_ptr<TrafficLightBrain> brain = chrono_types::make_shared<TrafficLightBrain>(1);
        agent->SetBrain(brain);



  


        // -----------------------------------
        // Sedan Agents (Rank: 2 to num_ranks)
        // -----------------------------------
        for (int i = 2; i < num_ranks; i++) {
            auto agent = chrono_types::make_shared<SynSedanAgent>(i);
            mpi_manager.AddAgent(agent, i);

            // Determine initial location and orientation depending on rank
            // TODO :: Find a better way to do this, maybe load from file or located in separate class

            // Called framework->To3DCartesian( first point in GPScoord vector ) to find these start points
            // 65.2568, -2.82435, 0.0670242
            // -84.8809, -123.315, 1.80245
            // -2.63172, 68.8964, 0.716005

            ChVector<> initLoc;
            ChQuaternion<> initRot;
            // rank = 3*rank - 2;
            int rankGroup = (rank + 2) % 4;

        //cout flag 2
        if(rank==0)
        {std::cout<< "This is flag 2 rank 0" << std::endl;}
        else if (rank == 1)
        {std::cout<< "This is flag 2 rank 1" << std::endl;}
        else if (rank == 2)
        {std::cout<< "This is flag 2 rank 2" << std::endl;}
        else if (rank == 3)
        {std::cout<< "This is flag 2 rank 3" << std::endl;}





            switch (rankGroup) {
                case 0:
                    // univ john inner
                    // University facing west, just before Park. Second lane from the left.
                    initLoc = ChVector<>({65.26 + (rank - 2) / 4 * 9, -2.82, 0.067});
                    initRot = ChQuaternion<>({-0.00942852, 0.00497593, -0.00215422, -0.999941});
                    std::cout<<"test point 1"<<std::endl;
                    break;
                case 1:
                    std::cout<<"test point 2 start"<<std::endl;
                    // Grainger loop
                    // Park St facing north, just before University. Left turn lane.
                    initLoc = ChVector<>({2, -40.0 - (rank - 3) / 4 * 9, 0.5});
                    initRot = ChQuaternion<>({-0.7071068, 0, 0, -0.7071068});
                    std::cout<<"test point 2"<<std::endl;
                    break;
                case 2:
                    std::cout<<"test point 3 start"<<std::endl;
                    // Park st. straight
                    // Park St facing south, just before University. Right turn lane.
                    initLoc = ChVector<>({-2.63, 69.0 + (rank - 4) / 3 * 9, 0.72});
                    initRot = ChQuaternion<>({-0.7071068, 0, 0, 0.7071068});
                    std::cout<<"test point 3"<<std::endl;
                    break;
                case 3:
                    std::cout<<"test point 4 start"<<std::endl;
                    // univ john outer
                    // University facing west, just before Park. Third lane from the left
                    initLoc = ChVector<>({109.56 + (rank - 5) / 4 * 9, 2.41, 0.11});
                    initRot = ChQuaternion<>({-0.00942852, 0.00497593, -0.00215422, -0.999941});
                    std::cout<<"test point 4"<<std::endl;
                    break;
                default:
                    std::cout<<"test point 5 start"<<std::endl;
                    std::cerr << "Unexpectedly reached default case statement" << std::endl;
                    // University facing west, just before Park. Second lane from the left.
                    initLoc = ChVector<>({75.0 - (rank - 1) * 9, -7.5, 0.5});
                    initRot = ChQuaternion<>({-0.00942852, 0.00497593, -0.00215422, -0.999941});
                    std::cout<<"test point 5"<<std::endl;
                    break;
            }
            // rank = (rank + 3)/3;

            std::cout<<"test point ends"<<std::endl;

            if (i == rank) {
                 std::cout<<"test point 6.5"<<std::endl;
                agent->InitializeVehicle(ChCoordsys<>(initLoc, initRot));
                std::cout<<"test point 6.6"<<std::endl;
                auto terrain = chrono_types::make_shared<SynRigidTerrain>(
                    
                    agent->GetSystem(), STRINGIFY(PARKST_COL_MESH_PATH), STRINGIFY(PARKST_VIS_MESH_PATH));
                std::cout<<"test point 6.7"<<std::endl;

                //std::cout<<"test point 6.65"<<std::endl;
                agent->SetTerrain(terrain);


                //test statement
                std::cout<<"test point 6 : "<<rank<<std::endl;



                agent->SetRank(rank);


        //cout flag 3
        if(rank==0)
        {std::cout<< "This is flag 3 rank 0" << std::endl;}
        else if (rank == 1)
        {std::cout<< "This is flag 3 rank 1" << std::endl;}
        else if (rank == 2)
        {std::cout<< "This is flag 3 rank 2" << std::endl;}
        else if (rank == 3)
        {std::cout<< "This is flag 3 rank 3" << std::endl;}


                //// Stuff for GPS paths on Park st.
                std::vector<GPScoord> gps_john_inner = {
                    GPScoord(43.0732367, -89.3998165), GPScoord(43.0732465, -89.4003892),
                    GPScoord(43.0732515, -89.4006265), GPScoord(43.0732583, -89.4009981),
                    GPScoord(43.0732632, -89.4013346), GPScoord(43.0732676, -89.4016545),
                    GPScoord(43.0732853, -89.4020133), GPScoord(43.0732824, -89.4022305),
                    GPScoord(43.0732588, -89.4023995), GPScoord(43.0731276, -89.4024397),
                    GPScoord(43.0730149, -89.4024558), GPScoord(43.0728209, -89.4024639),
                    GPScoord(43.0725453, -89.4024703), GPScoord(43.0722817, -89.4024730),
                    GPScoord(43.0721632, -89.4024663), GPScoord(43.0721328, -89.4021484),
                    GPScoord(43.0721279, -89.4018708), GPScoord(43.0721152, -89.4014095),
                    GPScoord(43.0721181, -89.4011440), GPScoord(43.0721181, -89.4009200),
                    GPScoord(43.0721064, -89.4004721), GPScoord(43.0721044, -89.4000519),
                    GPScoord(43.0720917, -89.3996764), GPScoord(43.0720927, -89.3993264),
                    GPScoord(43.0720848, -89.3989321), GPScoord(43.0720711, -89.3983313),
                    GPScoord(43.0720682, -89.3980081), GPScoord(43.0720692, -89.3978042),
                    GPScoord(43.0720721, -89.3975883), GPScoord(43.0722396, -89.3974341),
                    GPScoord(43.0724042, -89.3974354), GPScoord(43.0726767, -89.3974276),
                    GPScoord(43.0729040, -89.3974195), GPScoord(43.0730911, -89.3974195),
                    GPScoord(43.0731947, -89.3977805), GPScoord(43.0731907, -89.3981520),
                    GPScoord(43.0731917, -89.3986952), GPScoord(43.0732152, -89.3990150),
                    GPScoord(43.0732446, -89.3993704), GPScoord(43.0732456, -89.3995970)};
                std::vector<GPScoord> gps_grainger_loop = {
                    GPScoord(43.0721590, -89.4016810), GPScoord(43.0721541, -89.4015013),
                    GPScoord(43.0721531, -89.4013175), GPScoord(43.0721492, -89.4011378),
                    GPScoord(43.0721467, -89.4010379), GPScoord(43.0721448, -89.4008421),
                    GPScoord(43.0721683, -89.4007080), GPScoord(43.0722452, -89.4006731),
                    GPScoord(43.0723858, -89.4006396), GPScoord(43.0725440, -89.4006081),
                    GPScoord(43.0727282, -89.4005920), GPScoord(43.0728869, -89.4005987),
                    GPScoord(43.0730466, -89.4006054), GPScoord(43.0731181, -89.4006041),
                    GPScoord(43.0732131, -89.4006470), GPScoord(43.0732337, -89.4008213),
                    GPScoord(43.0732395, -89.4011593), GPScoord(43.0732454, -89.4014892),
                    GPScoord(43.0731984, -89.4017145), GPScoord(43.0730005, -89.4018003),
                    GPScoord(43.0727928, -89.4018057), GPScoord(43.0725087, -89.4018111),
                    GPScoord(43.0724362, -89.4018137), GPScoord(43.0723774, -89.4018084),
                    GPScoord(43.0723069, -89.4017923), GPScoord(43.0722266, -89.4017574),
                    GPScoord(43.0721952, -89.4017306), GPScoord(43.0721727, -89.4017051)};
                std::vector<GPScoord> gps_park_straight = {
                    GPScoord(43.0740081, -89.4006572), GPScoord(43.0738856, -89.4006639),
                    GPScoord(43.0737446, -89.4006693), GPScoord(43.0736192, -89.4006746),
                    GPScoord(43.0734595, -89.4006814), GPScoord(43.0732834, -89.4006908),
                    GPScoord(43.0731120, -89.4006800), GPScoord(43.0729572, -89.4006894),
                    GPScoord(43.0727730, -89.4006961), GPScoord(43.0724909, -89.4007149),
                    GPScoord(43.0722773, -89.4007658), GPScoord(43.0720863, -89.4008101),
                    GPScoord(43.0721010, -89.3998579), GPScoord(43.0742211, -89.3997015),
                    GPScoord(43.0742192, -89.4006550)};

                // jht - Original Park St. straight, trying a different one for now
                // GPScoord(43.0738876, -89.4006684), GPScoord(43.0737201, -89.4006705), GPScoord(43.0736167,
                // -89.4006705), GPScoord(43.0735016, -89.4006725), GPScoord(43.0733346, -89.4006899),
                // GPScoord(43.0731700, -89.4007207), GPScoord(43.0730083, -89.4007315), GPScoord(43.0728903,
                // -89.4007342), GPScoord(43.0727331, -89.4007449), GPScoord(43.0725910, -89.4007489),
                // GPScoord(43.0724881, -89.4007623), GPScoord(43.0722883, -89.4008106), GPScoord(43.0720267,
                // -89.4008803), GPScoord(43.0718778, -89.4009206), GPScoord(43.0716535, -89.4009501),
                // GPScoord(43.0714614, -89.4009514), GPScoord(43.0712733, -89.4009554), GPScoord(43.0711205,
                // -89.4009635), GPScoord(43.0708217, -89.4009702), GPScoord(43.0704739, -89.4009782),
                // GPScoord(43.0702506, -89.4009890), GPScoord(43.0700007, -89.4010064), GPScoord(43.0699351,
                // -89.4009085), GPScoord(43.0699929, -89.4007905), GPScoord(43.0701849, -89.4007757),
                // GPScoord(43.0704328, -89.4007690), GPScoord(43.0706983, -89.4007610), GPScoord(43.0708393,
                // -89.4007549), GPScoord(43.0710921, -89.4007563), GPScoord(43.0713155, -89.4007516),
                // GPScoord(43.0715364, -89.4007409), GPScoord(43.0718582, -89.4007207), GPScoord(43.0719577,
                // -89.4007060), GPScoord(43.0722310, -89.4006403), GPScoord(43.0724303, -89.4005860),
                // GPScoord(43.0726674, -89.4005551), GPScoord(43.0727776, -89.4005464), GPScoord(43.0729736,
                // -89.4005370), GPScoord(43.0730960, -89.4005316), GPScoord(43.0732645, -89.4005296),
                // GPScoord(43.0734365, -89.4005779), GPScoord(43.0735873, -89.4006081), GPScoord(43.0737646,
                // -89.4006249), GPScoord(43.0738988, -89.4006275), GPScoord(43.0740923, -89.4006242),
                // GPScoord(43.0745415, -89.4006188), GPScoord(43.0747110, -89.4006161), GPScoord(43.0750979,
                // -89.4006027), GPScoord(43.0753203, -89.4004405), GPScoord(43.0755750, -89.4004083),
                // GPScoord(43.0757562, -89.4005062), GPScoord(43.0757611, -89.4007301), GPScoord(43.0755916,
                // -89.4008173), GPScoord(43.0753516, -89.4007972), GPScoord(43.0750950, -89.4006631),
                // GPScoord(43.0747159, -89.4006577), GPScoord(43.0743896, -89.4006591), GPScoord(43.0741369,
                // -89.4006617), GPScoord(43.0739870, -89.4006658)};
                std::vector<GPScoord> gps_john_outer = {
                    GPScoord(43.0732603, -89.3992762), GPScoord(43.0732622, -89.3995713),
                    GPScoord(43.0732622, -89.3999200), GPScoord(43.0732700, -89.4002445),
                    GPScoord(43.0732720, -89.4005825), GPScoord(43.0732793, -89.4010022),
                    GPScoord(43.0732949, -89.4014850), GPScoord(43.0733028, -89.4019034),
                    GPScoord(43.0732910, -89.4023916), GPScoord(43.0729563, -89.4024530),
                    GPScoord(43.0726800, -89.4024450), GPScoord(43.0725017, -89.4024503),
                    GPScoord(43.0722901, -89.4024450), GPScoord(43.0721510, -89.4024530),
                    GPScoord(43.0721510, -89.4019568)};

                std::shared_ptr<SynFramework> framework =
                    chrono_types::make_shared<SynFramework>(GPScoord(43.073268, -89.400636), agent->GetTerrain());

                if (false && rank == 2) {
                    auto john_inner_point = framework->To3DCartesian(gps_john_inner[0], 0.5);
                    auto grainger_loop_point = framework->To3DCartesian(gps_grainger_loop[0], 0.5);
                    auto park_straight_point = framework->To3DCartesian(gps_park_straight[0], 0.5);
                    auto john_outer_point = framework->To3DCartesian(gps_john_outer[0], 0.5);

                    std::cout << john_inner_point.x() << ", " << john_inner_point.y() << ", " << john_inner_point.z()
                              << std::endl;
                    std::cout << grainger_loop_point.x() << ", " << grainger_loop_point.y() << ", "
                              << grainger_loop_point.z() << std::endl;
                    std::cout << park_straight_point.x() << ", " << park_straight_point.y() << ", "
                              << park_straight_point.z() << std::endl;
                    std::cout << john_outer_point.x() << ", " << john_outer_point.y() << ", " << john_outer_point.z()
                              << std::endl;
                }

                // Make paths from the GPS points
                std::shared_ptr<ChBezierCurve> path_john_inner = framework->CurveFromGPS(gps_john_inner, 0, true);
                std::shared_ptr<ChBezierCurve> path_grainger_loop = framework->CurveFromGPS(gps_grainger_loop, 0, true);
                std::shared_ptr<ChBezierCurve> path_park_straight = framework->CurveFromGPS(gps_park_straight, 0, true);
                std::shared_ptr<ChBezierCurve> path_john_outer = framework->CurveFromGPS(gps_john_outer, 0, true);

                std::shared_ptr<ChBezierCurve> path;
                int lane;
                switch (rankGroup) {
                    case 0:
                        path = path_john_inner;
                        lane = 3;
                        break;
                    case 1:
                        path = path_grainger_loop;
                        lane = 0;
                        break;
                    case 2:
                        path = path_park_straight;
                        lane = 6;
                        break;
                    case 3:
                        path = path_john_outer;
                        lane = 4;
                        break;
                    default:
                        path = path_john_inner;
                        lane = 3;
                        break;
                }

                double target_speed = 10;
                double target_following_time = 1.2;
                double target_min_distance = 10;
                double current_distance = 100;
                bool isPathClosed = false;

                if (rank == 4) {
                    target_min_distance = 13;
                }

                std::shared_ptr<ChPathFollowerACCDriver> driver = chrono_types::make_shared<ChPathFollowerACCDriver>(
                    agent->GetVehicle(), path, "Park St", target_speed, target_following_time, target_min_distance,
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

                std::shared_ptr<ACCBrain> brain =
                    chrono_types::make_shared<ACCBrain>(rank, driver, agent->GetVehicle());
                brain->setLane(lane);
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
                    auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>(SynSenVehicleVis::CAMERA);

                    auto manager = chrono_types::make_shared<ChSensorManager>(agent->GetSystem());
                    manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 2000);
                    manager->scene->AddPointLight({-100, 100, 100}, {1, 1, 1}, 2000);
                    sen_vis->SetSensorManager(manager);

                    auto origin = chrono_types::make_shared<ChBody>();
                    origin->SetBodyFixed(true);
                    agent->GetSystem()->AddBody(origin);

                    ChVector<> camera_loc(0, 0, 45);
                    // double theta = acos((camera_loc ^ chrono::VECT_X) / (camera_loc.Length() * 1));
                    // ChVector<> rot_axis = chrono::VECT_X % (-camera_loc);
                    // ChQuaternion<> rotation = Q_from_AngAxis(theta, rot_axis);

                    // Rotations to get a nice angle
                    ChQuaternion<> rotation = QUNIT;
                    // ChQuaternion<> qA = Q_from_AngAxis(45 * CH_C_DEG_TO_RAD, VECT_Y);
                    // ChQuaternion<> qB = Q_from_AngAxis(135 * CH_C_DEG_TO_RAD, VECT_Z);
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
