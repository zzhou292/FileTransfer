#include "synchrono/visualization/SynSenVehicleVis.h"

namespace synchrono {
namespace visualization {

SynSenVehicleVis::SynSenVehicleVis(SensorVisType type)
    : SynVis(SynVis::SENSOR_VIS), m_sensor_type(type), m_needs_reconstruction(true) {}

void SynSenVehicleVis::Update(double step) {
    if (m_needs_reconstruction) {
        m_sensor_manager->ReconstructScenes();
        m_needs_reconstruction = false;
    }
    m_sensor_manager->Update();
}

void SynSenVehicleVis::Initialize() {
    if (m_sensor_manager == nullptr) {
        std::cout << "SynSenVehicleVis::Initialize: Sensor manager has not been initialized. Must initialize prior "
                     "to adding sensor. Exitting..."
                  << std::endl;
        exit(-1);
    } else if (m_sensor == nullptr) {
        // std::cout
        //     << "SynSenVehicleVis::Initialize: Sensor has not been created or attached. Must attach a sensor prior "
        //        "to initializing the sensor vis. Exitting..."
        //     << std::endl;
        // exit(-1);
        return;
    }
    m_sensor_manager->AddSensor(m_sensor);
}

void SynSenVehicleVis::InitializeDefaultSensorManager(ChSystem* system) {
    if (m_sensor_manager == nullptr) {
        // create the sensor manager and a camera
        m_sensor_manager = chrono_types::make_shared<ChSensorManager>(system, true);

        // add default lights
        m_sensor_manager->scene->AddPointLight({100, 100, 100}, {1, 1, 1}, 5000);
        m_sensor_manager->scene->AddPointLight({-100, -100, 100}, {1, 1, 1}, 5000);
    } else {
        std::cerr << "SynVisManager::InitializeDefaultSensorManager: Sensor manager has already been initialized. "
                     "Ignoring..."
                  << std::endl;
    }
}

void SynSenVehicleVis::InitializeAsDefaultChaseCamera(std::shared_ptr<ChBodyAuxRef> body_ref,
                                                      unsigned int width,
                                                      unsigned int height,
                                                      float fps,
                                                      std::string window_title) {
    m_sensor = chrono_types::make_shared<ChCameraSensor>(
        body_ref,                                                                      // body camera is attached to
        fps,                                                                           // update rate in Hz
        chrono::ChFrame<double>({-8, 0, 3}, Q_from_AngAxis(CH_C_PI / 20, {0, 1, 0})),  // offset pose
        width,                                                                         // image width
        height,                                                                        // image height
        CH_C_PI / 3,                                                                   // horizontal field of view
        1 / fps,                                                                       // camera lag
        0);                                                                            // exposure time
    m_sensor->SetName(window_title);
}

void SynSenVehicleVis::InitializeAsDefaultBirdsEyeCamera(std::shared_ptr<ChBodyAuxRef> body_ref,
                                                         double z,
                                                         unsigned int width,
                                                         unsigned int height,
                                                         float fps,
                                                         std::string window_title) {
    m_sensor = chrono_types::make_shared<ChCameraSensor>(
        body_ref,                                                                     // body camera is attached to
        fps,                                                                          // update rate in Hz
        chrono::ChFrame<double>({0, 0, z}, Q_from_AngAxis(CH_C_PI / 2., {0, 1, 0})),  // offset pose
        width,                                                                        // image width
        height,                                                                       // image height
        CH_C_PI / 3,                                                                  // horizontal field of view
        1 / fps,                                                                      // camera lag
        0);                                                                           // exposure time
    m_sensor->SetName(window_title);
}

void SynSenVehicleVis::AddFilterRGBA8Access() {
    auto camera = std::static_pointer_cast<ChCameraSensor>(m_sensor);
    if (camera) {
        camera->PushFilter(chrono_types::make_shared<ChFilterRGBA8Access>());
    } else {
        std::cout << "SynSenVehicleVis::AddFilterRGBA8Access: Attached sensor is not a camera. Ignoring..."
                  << std::endl;
    }
}

void SynSenVehicleVis::AddFilterVisualize(unsigned int w, unsigned int h) {
    auto camera = std::static_pointer_cast<ChCameraSensor>(m_sensor);
    if (camera) {
        camera->PushFilter(chrono_types::make_shared<ChFilterVisualize>(w, h));
        return;
    }
    auto lidar = std::static_pointer_cast<ChLidarSensor>(m_sensor);
    if (lidar) {
        lidar->PushFilter(chrono_types::make_shared<ChFilterVisualize>(w, h));
        return;
    }
    std::cout << "SynSenVehicleVis::AddFilterVisualize: Attached sensor is not a camera or a lidar. Ignoring..."
              << std::endl;
}

void SynSenVehicleVis::AddFilterSave(std::string file_path) {
    auto camera = std::static_pointer_cast<ChCameraSensor>(m_sensor);
    if (camera) {
        camera->PushFilter(chrono_types::make_shared<ChFilterSave>(file_path));
        return;
    }
    auto lidar = std::static_pointer_cast<ChLidarSensor>(m_sensor);
    if (lidar) {
        lidar->PushFilter(chrono_types::make_shared<ChFilterSave>(file_path));
        return;
    }
    std::cout << "SynSenVehicleVis::AddFilterSave: Attached sensor is not a camera or a lidar. Ignoring..."
              << std::endl;
}
}  // namespace visualization
}  // namespace synchrono
