#include "synchrono/SynMPIManager.h"
#include "synchrono/SynFramework.h"
#include "synchrono/SynVisManager.h"
#include "synchrono/SynCLI.h"

#include "synchrono/agent/SynHMMWVAgent.h"
#include "synchrono/agent/SynFullHMMWVAgent.h"
#include "synchrono/visualization/SynIrrVehicleVis.h"
#include "synchrono/visualization/SynSenVehicleVis.h"

#include "synchrono/terrain/SynSCMTerrain.h"
#include "synchrono/terrain/SynRigidTerrain.h"

#include "InferenceDriver.h"
#include "SynInferenceBrain.h"
#include "SynV2VBrain.h"

#include "chrono/geometry/ChTriangleMeshConnected.h"

#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/ChGPSSensor.h"
#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_vehicle/driver/ChPathFollowerACCDriver.h"
#include "chrono/core/ChBezierCurve.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono/physics/ChBodyEasy.h"

#include <chrono>
#include <stdlib.h>
#include <math.h>

using namespace synchrono;
using namespace synchrono::agent;
using namespace synchrono::mpi;

using namespace chrono;
using namespace chrono::sensor;
using namespace chrono::geometry;

// =============================================================================

// Create scm terrain from path
std::shared_ptr<SynTerrain> CreateTerrainFromPath(std::shared_ptr<SynVehicleAgent> agent,
                                                  int sizeX,
                                                  int sizeY,
                                                  std::shared_ptr<ChBezierCurve> path,
                                                  double width,
                                                  double divs,
                                                  bool is_scm,
                                                  bool is_hard = false) {
    // Get the total length of the path (approximate)
    float total_length = 0;
    float interval = 0.001;
    ChVector<> v0 = path->eval(0);
    ChVector<> v1 = path->eval(interval);
    for (float i = interval; i < 1; i += interval) {
        total_length += (v1 - v0).Length();
        v0 = path->eval(i);
        v1 = path->eval(i + interval);
    }
    double dx = 1. / double(divs);
    double dy = dx / total_length;
    // Initialze terrain
    if (is_scm) {
        // Initialize the scm terrain
        // Use "soft" parameters
        SCMParameters params;
        if (is_hard)
            params.InitializeParametersAsMid();
        else
            params.InitializeParametersAsSoft();

        auto terrain =
            chrono_types::make_shared<SynSCMTerrain>(agent->GetSystem(), sizeX, sizeY, path, width, dx, dy, &params);

        // Add a moving patch for the chassis
        // terrain->GetSCMTerrain()->AddMovingPatch(agent->GetVehicle().GetChassisBody(), ChVector<>(0, 0, 0), 5, 5);

        // Add texture for the terrain
        auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
        vis_mat->SetSpecularColor({.1f, .1f, .1f});
        // vis_mat->SetKdTexture(GetChronoDataFile("vehicle/terrain/textures/grass.jpg"));
        vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
        // vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/snow.jpg"));
        terrain->GetSCMTerrain()->GetMesh()->material_list.push_back(vis_mat);
        MaterialInfo minfo;
        minfo.mu = 0.9f;
        minfo.cr = 0.01f;
        minfo.Y = 2e7f;
        auto patch_mat = minfo.CreateMaterial(ChContactMethod::NSC);
        auto box = chrono_types::make_shared<ChBodyEasyBox>(sizeX * 2, sizeY * 2, 0, 2700, true, false);
        auto patch_asset = box->GetAssets()[0];
        if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(patch_asset)) {
            std::shared_ptr<ChVisualMaterial> box_texture = chrono_types::make_shared<ChVisualMaterial>();
            // box_texture->SetKdTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"));
            box_texture->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
            // box_texture->SetKdTexture(GetChronoDataFile("sensor/textures/snow.jpg"));
            // FresnelMax and SpecularColor should make it less shiny
            box_texture->SetFresnelMax(0.2);
            box_texture->SetSpecularColor({0.2, 0.2, 0.2});

            visual_asset->material_list.push_back(box_texture);
        }

        return terrain;
    } else {
        auto terrain =
            chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), sizeX, sizeY, path, 0, width, 0.05, 0.05);
        return terrain;
    }
}

// Create scm terrain
std::shared_ptr<SynTerrain> CreateTerrain(std::shared_ptr<SynVehicleAgent> agent,
                                          int sizeX,
                                          int sizeY,
                                          bool is_scm,
                                          int divs = 20,
                                          bool is_hard = false) {
    // Initialze terrain
    if (is_scm) {
        // Initialize the scm terrain
        // Use "soft" parameters
        SCMParameters params;
        if (is_hard)
            params.InitializeParametersAsMid();
        else
            params.InitializeParametersAsSoft();
        double divX = sizeX * divs;
        double divY = sizeY * divs;
        auto terrain = chrono_types::make_shared<SynSCMTerrain>(agent->GetSystem(), sizeX, sizeY, divX, divY, &params);

        // Add a moving patch for the chassis
        terrain->GetSCMTerrain()->AddMovingPatch(agent->GetVehicle().GetChassisBody(), ChVector<>(0, 0, 0), 5, 5);

        // Add texture for the terrain
        auto vis_mat = chrono_types::make_shared<ChVisualMaterial>();
        vis_mat->SetSpecularColor({.1f, .1f, .1f});
        // vis_mat->SetKdTexture(GetChronoDataFile("vehicle/terrain/textures/grass.jpg"));
        // vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/grass_texture.jpg"));
        if (is_hard)
            vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/mud.png"));
        else
            vis_mat->SetKdTexture(GetChronoDataFile("sensor/textures/snow.jpg"));
        terrain->GetSCMTerrain()->GetMesh()->material_list.push_back(vis_mat);

        return terrain;
    } else {
        auto terrain = chrono_types::make_shared<SynRigidTerrain>(agent->GetSystem(), 0, sizeX * 1.5, sizeY * 1.5);
        return terrain;
    }
}

// =============================================================================

int main(int argc, char* argv[]) {
    // CLI tools for default synchrono demos
    SynCLI cli(argv[0]);
    cli.AddDefaultDemoOptions();
    // Add custom options
    cli.AddOption("Demo", cxxopts::Option("L,leader", "Ranks that should be considered leaders",
                                          cxxopts::value<std::vector<int>>()->default_value("-1")));
    cli.AddOption("Demo",
                  cxxopts::Option("S,scm", "Toggle scm terrain on", cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo", cxxopts::Option("H,hard", "Toggle hard scm terrain on",
                                          cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo",
                  cxxopts::Option("hdr", "Toggle hdr background on", cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo", cxxopts::Option("D,divs", "Number of nodes per 1 meter in along a single axis",
                                          cxxopts::value<int>()->default_value("20")));
    cli.AddOption("Demo", cxxopts::Option("X,sizeX", "Size of terrain in the X direction",
                                          cxxopts::value<int>()->default_value("100")));
    cli.AddOption("Demo", cxxopts::Option("Y,sizeY", "Size of terrain in the Y direction",
                                          cxxopts::value<int>()->default_value("100")));
    cli.AddOption("Demo",
                  cxxopts::Option("w,width", "Width of terrain path", cxxopts::value<int>()->default_value("5")));
    cli.AddOption("Demo",
                  cxxopts::Option("random", "Toggle random on", cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo",
                  cxxopts::Option("o,output", "Toggle output on", cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor", cxxopts::Option("B,birds_eye", "Toggle birds eye view on",
                                            cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor", cxxopts::Option("T,third_person", "Toggle third person view on",
                                            cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor",
                  cxxopts::Option("tire_view", "Toggle tire view on", cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Sensor", cxxopts::Option("P,path", "Path to where images will be saved",
                                            cxxopts::value<std::string>()->default_value("SENSOR_OUTPUT/")));
    cli.AddOption("Sensor", cxxopts::Option("r,resolution", "Resolution of image (WIDTH,HEIGHT)",
                                            cxxopts::value<std::vector<int>>()->default_value("600,400")));
    cli.AddOption("Sensor",
                  cxxopts::Option("use_path", "Use path for terrain", cxxopts::value<bool>()->default_value("false")));
    cli.AddOption("Demo", cxxopts::Option("p,path_num", "Path number", cxxopts::value<int>()->default_value("1")));
    cli.AddOption("Demo", cxxopts::Option("I,init_pos", "Initial parameter along path",
                                          cxxopts::value<double>()->default_value("0.01")));
    cli.AddOption("Demo", cxxopts::Option("interval", "Interval parameter along path",
                                          cxxopts::value<double>()->default_value("0.05")));

    cli.Parse(argc, argv);

    const double T_END = cli.GetAsType<double>("time_end");
    const double HEARTBEAT = cli.GetAsType<double>("heartbeat");
    const double STEP_SIZE = cli.GetAsType<double>("step_size");
    const bool verbose = cli.GetAsType<bool>("verbose");
    const bool RANDOM = cli.GetAsType<bool>("random");
    const bool OUTPUT = cli.GetAsType<bool>("output");
    const std::string out_dir = cli.GetAsType<std::string>("path");

    // GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    vehicle::SetDataPath(CHRONO_VEHICLE_DATA_DIR);

    // Create path to follow and place obstacles around
    std::vector<ChVector<>> points;
    switch (cli.GetAsType<int>("path_num")) {
        case 1:
            points = {{-40, -90, 0.5}, {-40, -30, 0.5}, {0, -30, 0.5}, {0, 30, 0.5}, {40, 30, 0.5}, {40, 90, 0.5}};
            break;
        case 2:
            points = {{-40, -90, 0.5}, {-40, -30, 0.5}, {-40, 0, 0.5}, {-40, 30, 0.5}, {40, 45, 0.5}, {40, 90, 0.5}};
            break;
        case 3:
            points = {{-40, -90, 0.5}, {-40, -30, 0.5}, {-40, 0, 0.5}, {40, 0, 0.5}, {40, 30, 0.5}, {40, 90, 0.5}};
            break;
        case 4:
            points = {{-40, -90, 0.5}, {-20, -30, 0.5}, {0, 0, 0.5}, {20, 30, 0.5}, {40, 60, 0.5}, {40, 90, 0.5}};
            break;
        case 5:
            points = {{-40, -50, 0.5}, {40, -30, 0.5}, {10, 10, 0.5}, {-10, 20, 0.5}, {40, 40, 0.5}, {40, 90, 0.5}};
            break;
        default:
            std::cout << "Path type unknown. Exitting..." << std::endl;
            exit(-1);
    }
    auto path = chrono_types::make_shared<ChBezierCurve>(points);

    // Helper method for determining current pose of t parameter along a ChBezierCurve
    auto CalcPose = [path](double t, double z = 0.75) -> ChCoordsys<> {
        auto pos = path->eval(t);
        pos.z() = z;
        // Param-only derivative
        double par = t;
        ChClampValue(par, 0.0, 1.0);
        int numIntervals = path->getNumPoints() - 1;
        double epar = par * numIntervals;
        int td = floor(epar);
        ChClampValue(td, 0, numIntervals - 1);
        auto posD = path->evalD((int)td, epar - td);

        auto alpha = atan2(posD.y(), posD.x());
        auto rot = ChQuaternion<>();
        rot.Q_from_AngZ(alpha);
        return ChCoordsys<>(pos, rot);
    };

    // For timing
    double wall_time = 0;

    // ------------------
    // MPI Initialization
    // ------------------
    SynMPIManager mpi_manager(argc, argv);
    int rank = mpi_manager.GetRank();
    int num_ranks = mpi_manager.GetNumRanks();

    // Check if rank is a leader as specified by the user
    bool rank_is_leader = cli.HasValueInVector<int>("leader", rank);

    // "global" variables used in the loop
    std::shared_ptr<SynFullHMMWVAgent> agent;
    double target_speed = 0.0;
    double dv = 0.0007038873110933874 * 2;

    // Initialize output file for position data
    std::string output_path = out_dir + "/output" + std::to_string(rank) + ".txt";
    chrono::utils::CSV_writer log_csv(" ");

    if (rank != 0) {
        // --------------------
        // Agent Initialization
        // --------------------
        agent = chrono_types::make_shared<SynFullHMMWVAgent>(rank);
        agent->SetStepSize(STEP_SIZE);
        mpi_manager.AddAgent(agent, rank);

        // Position hmmwv vehicles
        // Starts closest to beginning of path and moves forward along path at specified interval
        ChCoordsys<> initPose;
        // double initT = .1744067519636624;
        double initT = cli.GetAsType<double>("init_pos");
        double interval = cli.GetAsType<double>("interval");
        initPose = CalcPose(initT + rank * interval);

        for (int i = 1; i < num_ranks; i++) {
            if (i != rank)
                mpi_manager.AddAgent(chrono_types::make_shared<SynHMMWVAgent>(i, agent->GetSystem()), i);
        }

        // Terrain parameters across all agents
        double sizeX = cli.GetAsType<int>("sizeX");
        double sizeY = cli.GetAsType<int>("sizeY");
        double width = cli.GetAsType<int>("width");
        bool use_path = cli.GetAsType<bool>("use_path");

        // Common sensor parameters
        int gps_update_rate = 100;
        float gps_lag = 1. / gps_update_rate;
        float gps_collection_window = 0;
        ChVector<> origin(-89.400, 43.070, 260.0);
        auto gps_noise_none = chrono_types::make_shared<ChGPSNoiseNone>();

        // Create visualization manager which will faciliate sensor updates and irrlicht visualization (if needed)
        auto vis_manager = chrono_types::make_shared<SynVisManager>();
        agent->AttachVisManager(vis_manager);

        // Create the sensor manager and setup the environment
        std::shared_ptr<ChSensorManager> sensor_manager;

        // Ranks can either be have "intelligent" (knowledge of the goal and path to follower) or inference drivers
        // User specifies which ranks should be leaderes, all others will use inference policy based on camera and gps
        // input
        if (rank_is_leader) {
            // Create a leader agent

            // Initialize the vehicle
            agent->InitializeVehicle(initPose);

            // Initialize the terrain
            std::shared_ptr<SynTerrain> terrain;
            if (use_path)
                terrain = CreateTerrainFromPath(agent, sizeX, sizeY, path, width, cli.GetAsType<int>("divs"),
                                                cli.GetAsType<bool>("scm"), cli.GetAsType<bool>("hard"));
            else
                terrain = CreateTerrain(agent, sizeX, sizeY, cli.GetAsType<bool>("scm"), cli.GetAsType<int>("divs"),
                                        cli.GetAsType<bool>("hard"));
            agent->SetTerrain(terrain);

            // Create the adaptive cruise control follower
            auto driver = chrono_types::make_shared<ChPathFollowerACCDriver>(agent->GetVehicle(), path, "",
                                                                             target_speed, 1.2, 3, 100, false);
            driver->GetSteeringController().SetLookAheadDistance(5);
            driver->GetSteeringController().SetGains(0.8, 0, 0);
            driver->GetSpeedController().SetGains(0.4, 0, 0);
            driver->Initialize();

            // Initialize sensor manager and add point lights
            sensor_manager = chrono_types::make_shared<ChSensorManager>(agent->GetSystem());
            sensor_manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 5000);
            sensor_manager->scene->AddPointLight({-50, -50, 100}, {1, 1, 1}, 2500);

            // GPS sensor for each leader vehicle
            auto gps =
                chrono_types::make_shared<ChGPSSensor>(agent->GetVehicle().GetChassisBody(),  // body gps is attached to
                                                       gps_update_rate,                       // update rate in Hz
                                                       ChFrame<>({0, 0, 0}, {1, 0, 0, 0}),    // offset pose
                                                       gps_lag,                               // gps lag
                                                       gps_collection_window,                 // collection time
                                                       origin,                                // reference GPS location
                                                       gps_noise_none                         // gps noise model
                );
            gps->PushFilter(chrono_types::make_shared<ChFilterGPSAccess>());
            sensor_manager->AddSensor(gps);

            // Create brain
            auto brain = chrono_types::make_shared<SynV2VBrain>(rank, driver, agent->GetVehicle(), gps);
            agent->SetBrain(brain);

            // Attach sensor manager to visualization manager so that it gets updated during the simulation
            auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
            sen_vis->SetSensorManager(sensor_manager);
            vis_manager->AttachVis(sen_vis);
        } else {
            // Create inference agent

            // Initialze vehicle
            // Create a custom hmmwv setup that is different from the default parameters in the agent class
            auto hmmwv = agent->GetHMMWV();
            hmmwv->SetContactMethod(ChContactMethod::NSC);
            hmmwv->SetChassisCollisionType(ChassisCollisionType::PRIMITIVES);
            hmmwv->SetPowertrainType(PowertrainModelType::SHAFTS);
            hmmwv->SetDriveType(DrivelineType::AWD);
            hmmwv->SetChassisFixed(false);
            hmmwv->SetInitPosition(initPose);
            hmmwv->SetTireType(TireModelType::RIGID);
            hmmwv->SetTireStepSize(STEP_SIZE);
            hmmwv->Initialize();

            hmmwv->SetChassisVisualizationType(VisualizationType::PRIMITIVES);
            hmmwv->SetSuspensionVisualizationType(VisualizationType::PRIMITIVES);
            hmmwv->SetSteeringVisualizationType(VisualizationType::PRIMITIVES);
            hmmwv->SetWheelVisualizationType(VisualizationType::MESH);
            hmmwv->SetTireVisualizationType(VisualizationType::MESH);
            agent->SetSystem(hmmwv->GetSystem());

            // Initialze terrain
            std::shared_ptr<SynTerrain> terrain;
            if (use_path)
                terrain = CreateTerrainFromPath(agent, sizeX, sizeY, path, width, cli.GetAsType<int>("divs"),
                                                cli.GetAsType<bool>("scm"), cli.GetAsType<bool>("hard"));
            else
                terrain = CreateTerrain(agent, sizeX, sizeY, cli.GetAsType<bool>("scm"), cli.GetAsType<int>("divs"),
                                        cli.GetAsType<bool>("hard"));
            agent->SetTerrain(terrain);

            // Initialize sensor manager and add point lights
            sensor_manager = chrono_types::make_shared<ChSensorManager>(agent->GetSystem());
            sensor_manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 5000);
            sensor_manager->scene->AddPointLight({-100, -100, 100}, {1, 1, 1}, 5000);
            if (cli.GetAsType<bool>("hdr")) {
                sensor_manager->scene->GetBackground().has_texture = true;
                sensor_manager->scene->GetBackground().env_tex = "sensor/textures/winter_lake_01_8k.hdr";
                // sensor_manager->scene->GetBackground().env_tex = "sensor/textures/qwantani_8k.hdr";
                sensor_manager->scene->GetBackground().has_changed = true;
            }

            // Create sensors to be used during inference
            // Camera attached to the front of agent
            int camera_update_rate = 30;  // Camera update rate [Hz]
            int camera_width = 80;        // Camera width [pixels]
            int camera_height = 45;       // Camera height [pixels]
            auto camera = chrono_types::make_shared<ChCameraSensor>(
                agent->GetVehicle().GetChassisBody(),     // body camera is attached to
                camera_update_rate,                       // update rate in Hz
                ChFrame<>({1.5, 0, .875}, {1, 0, 0, 0}),  // offset pose
                camera_width,                             // image width
                camera_height,                            // image height
                CH_C_PI / 3.,                             // horizontal field of view
                1. / camera_update_rate,                  // camera lag
                0                                         // exposure time
            );
            camera->SetName("Camera Sensor");
            camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());
            sensor_manager->AddSensor(camera);

            // This agents GPS sensor
            auto gps =
                chrono_types::make_shared<ChGPSSensor>(agent->GetVehicle().GetChassisBody(),  // body gps is attached to
                                                       gps_update_rate,                       // update rate in Hz
                                                       ChFrame<>({0, 0, 0}, {1, 0, 0, 0}),    // offset pose
                                                       gps_lag,                               // gps lag
                                                       gps_collection_window,                 // collection time
                                                       origin,                                // reference GPS location
                                                       gps_noise_none                         // gps noise model
                );
            gps->PushFilter(chrono_types::make_shared<ChFilterGPSAccess>());
            sensor_manager->AddSensor(gps);

            // Create inference driver
            auto driver = chrono_types::make_shared<InferenceDriver>(
                agent->GetVehicle(), GetChronoDataFile("sensor/neural_nets/gvsets_early_fusion.onnx"), camera, gps);
            driver->Initialize();

            // Create brain
            auto brain = chrono_types::make_shared<SynInferenceBrain>(rank, driver, agent->GetVehicle(), gps);
            agent->SetBrain(brain);

            // Attach sensor manager to visualization manager so that it gets updated during the simulation
            auto sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
            sen_vis->SetSensorManager(sensor_manager);
            vis_manager->AttachVis(sen_vis);
        }

        // All ranks need obstacles present (even path follower for visualization purposes)
        int num_obstacles = 10;
        std::vector<std::string> obstacle_paths;
        if (cli.GetAsType<bool>("scm") && !cli.GetAsType<bool>("hard")) {
            obstacle_paths = {"sensor/offroad/rock1.obj", "sensor/offroad/rock3.obj", "sensor/offroad/rock4.obj",
                              "sensor/offroad/rock5.obj"};
        } else {
            obstacle_paths = {"sensor/offroad/rock1.obj", "sensor/offroad/rock3.obj", "sensor/offroad/rock4.obj",
                              "sensor/offroad/rock5.obj", "sensor/offroad/tree1.obj", "sensor/offroad/bush.obj"};
        }

        std::vector<int> sides = {1, 0, 1, 1, 0, 0, 0, 1, 1, 0};

        std::vector<ChVector<>> obstacle_bounding_boxes = {{3.18344, 3.62827, 0}, {2.53149, 2.48862, 0},
                                                           {2.4181, 4.47276, 0},  {3.80205, 2.56996, 0},
                                                           {2.39271, 2.36872, 0}, {1.35348, 1.33575, 0}};
        std::vector<std::shared_ptr<ChBody>> bodies(num_obstacles);

        for (int i = 0; i < num_obstacles; i++) {
            // Generate random mesh and its pose (position, orientation)
            int random_index = 0;
            if (RANDOM)
                random_index = rand() % obstacle_paths.size();  // random number between 0 and 5
            auto obstacle_path = obstacle_paths[random_index];
            auto coord_sys = CalcPose((i + 1) / (double)(num_obstacles + 1));

            // Randomly place object on left or right side of path
            int side = 1;
            if (RANDOM || true)
                side = rand() % 2 + 1;  // randomly select 1 or 2
            // side = sides[i];
            int dist = 8;
            coord_sys.pos = coord_sys.pos + coord_sys.rot.Rotate(VECT_Y) * (dist * pow(-1, side)) -
                            ChVector<>(0, 0, coord_sys.pos.z());

            // bool should_break = false;
            // for (auto point : points) {
            //     std::cout << (coord_sys.pos - point).Length() << std::endl;
            //     if ((coord_sys.pos - point).Length() < 10) {
            //         should_break = true;
            //         break;
            //     }
            // }
            // if (should_break)
            //     break;

            auto mesh = chrono_types::make_shared<ChTriangleMeshConnected>();
            mesh->LoadWavefrontMesh(GetChronoDataFile(obstacle_path), false, true);

            auto shape = chrono_types::make_shared<ChTriangleMeshShape>();
            shape->SetMesh(mesh);
            shape->SetName("Obstacle");
            shape->SetStatic(true);

            auto body = chrono_types::make_shared<ChBody>();
            body->SetPos({coord_sys.pos});  // Only using position for now
            body->AddAsset(shape);
            body->SetBodyFixed(true);

            // Create a rectangular collision model for the obstacle
            auto material_surface = chrono_types::make_shared<ChMaterialSurfaceSMC>();
            auto bb = obstacle_bounding_boxes[random_index] / 2;
            body->GetCollisionModel()->ClearModel();
            body->GetCollisionModel()->AddBox(material_surface, bb.x(), bb.y(), bb.z());
            body->GetCollisionModel()->BuildModel();
            body->SetCollide(true);

            // Add body to the ChSystem
            agent->GetSystem()->Add(body);
            bodies[i] = body;
        }

        // Save images cause Aaron doesn't have a NVIDIA GPU and he must work remotely :(
        if (cli.HasValueInVector<int>("sens", rank)) {
            std::vector<int> res = cli.GetAsType<std::vector<int>>("resolution");
            int w = res[0];
            int h = res[1];
            if (cli.GetAsType<bool>("third_person")) {
                // Third person camera
                auto third_sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
                third_sen_vis->SetSensorManager(sensor_manager);
                third_sen_vis->InitializeAsDefaultChaseCamera(agent->GetVehicle().GetChassisBody(), w, h);
                if (cli.GetAsType<bool>("sens_save")) {
                    std::string filename = out_dir + "third_person" + std::to_string(rank) + "/";
                    third_sen_vis->AddFilterSave(filename);
                }
                if (cli.GetAsType<bool>("sens_vis"))
                    third_sen_vis->AddFilterVisualize();
                vis_manager->AttachVis(third_sen_vis);
            }

            // Birds eye view camera
            if (cli.GetAsType<bool>("birds_eye")) {
                auto body = chrono_types::make_shared<ChBodyAuxRef>();
                body->SetBodyFixed(true);
                agent->GetSystem()->AddBody(body);
                auto bird_sen_vis = chrono_types::make_shared<SynSenVehicleVis>();
                bird_sen_vis->SetSensorManager(sensor_manager);
                bird_sen_vis->InitializeAsDefaultBirdsEyeCamera(body, 200, w, h);
                if (cli.GetAsType<bool>("sens_save")) {
                    std::string filename = out_dir + "birds_eye" + std::to_string(rank) + "/";
                    bird_sen_vis->AddFilterSave(filename);
                }
                if (cli.GetAsType<bool>("sens_vis"))
                    bird_sen_vis->AddFilterVisualize();
                vis_manager->AttachVis(bird_sen_vis);
            }

            // Tire view camera
            if (cli.GetAsType<bool>("tire_view")) {
                // Third person camera
                auto tire_view_vis = chrono_types::make_shared<SynSenVehicleVis>();
                tire_view_vis->SetSensorManager(sensor_manager);
                ChQuaternion<> q = Q_from_AngZ(-CH_C_PI * 3 / 4);
                auto camera = chrono_types::make_shared<ChCameraSensor>(
                    agent->GetVehicle().GetChassisBody(),       // body camera is attached to
                    30,                                         // update rate in Hz
                    chrono::ChFrame<double>({4.5, 3, .35}, q),  // offset pose
                    w,                                          // image width
                    h,                                          // image height
                    CH_C_PI / 3.,                               // horizontal field of view
                    1. / 30.,                                   // camera lag
                    0                                           // exposure time
                );
                camera->SetName("Camera Sensor");
                tire_view_vis->SetSensor(camera);
                if (cli.GetAsType<bool>("sens_save")) {
                    std::string filename = out_dir + "tire_view" + std::to_string(rank) + "/";
                    tire_view_vis->AddFilterSave(filename);
                }
                if (cli.GetAsType<bool>("sens_vis"))
                    tire_view_vis->AddFilterVisualize();
                vis_manager->AttachVis(tire_view_vis);
            }
        }
    }

    // Wait for all agents to catch up
    mpi_manager.Barrier();
    // Initalize zombie agents
    mpi_manager.Initialize();
    mpi_manager.Barrier();

    double step = 0;
    int updates = 0;

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

        if (rank_is_leader) {
            target_speed += dv;
            std::static_pointer_cast<ChPathFollowerACCDriver>(agent->GetDriver())->SetDesiredSpeed(target_speed);
            // std::cout << "Speed :: " << target_speed << std::endl;
        }

        if (OUTPUT && rank) {
            auto pos = agent->GetVehicle().GetChassisBody()->GetPos();
            auto vel = agent->GetVehicle().GetChassisBody()->GetPos_dt();
            auto acc = agent->GetVehicle().GetChassisBody()->GetPos_dtdt();
            log_csv << rank;
            log_csv << agent->GetSystem()->GetChTime();
            log_csv << pos.x() << pos.y() << pos.z();
            log_csv << vel.x() << vel.y() << vel.z();
            log_csv << acc.x() << acc.y() << acc.z();
            log_csv << std::endl;
        }

        if (step * HEARTBEAT - updates > 0) {
            if (rank == 0) {
                std::cout << "Sim Time :: " << step * HEARTBEAT << std::endl;
                updates++;
            } else if (OUTPUT) {
                log_csv.write_to_file(output_path);
            }
        }
    }

    if (rank == 0) {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "Fraction of real time: " << (time_span.count() / 1e6) / T_END << std::endl;
    }

    return 0;
}
