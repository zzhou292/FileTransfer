#ifndef SYN_SENSOR_VIS_H
#define SYN_SENSOR_VIS_H

#include "synchrono/SynApi.h"

#include "synchrono/SynVis.h"

#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/ChSensor.h"
#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/ChLidarSensor.h"
#include "chrono_sensor/filters/ChFilterAccess.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterSave.h"

#include <string>
#include <iostream>

using namespace chrono;
using namespace chrono::sensor;

namespace synchrono {
namespace visualization {

class SYN_API SynSenVehicleVis : public SynVis {
  public:
    /// Visualization types
    enum SensorVisType {
        CAMERA,  ///> Camera sensor visualization
        LIDAR    ///> Irrlicht visualization
    };

    /// Constructs a sensor vis
    SynSenVehicleVis() : SynVis(SynVis::SENSOR_VIS), m_needs_reconstruction(true) {}
    SynSenVehicleVis(SensorVisType type);

    /// Destructor
    ~SynSenVehicleVis() {}

    /// Advance the state of this visualizer.
    /// ChSensorManager will only update a sensor if needed
    virtual void Update(double step) override;

    /// Initialize the sensor visualizer
    /// Basically just adds the sensor to the sensor manager, if possible
    virtual void Initialize() override;

    /// Initializes a default sensor manager
    void InitializeDefaultSensorManager(ChSystem* system);

    /// Creates and attaches a default chase (third person) camera sensor
    void InitializeAsDefaultChaseCamera(std::shared_ptr<ChBodyAuxRef> body_ref,
                                        unsigned int width = 1280,
                                        unsigned int height = 720,
                                        float fps = 30,
                                        std::string window_title = "Default Chase Camera Sensor");

    /// Creates and attaches a default birds eye view camera sensor
    void InitializeAsDefaultBirdsEyeCamera(std::shared_ptr<ChBodyAuxRef> body_ref,
                                           double z = 285,
                                           unsigned int width = 1280,
                                           unsigned int height = 720,
                                           float fps = 30,
                                           std::string window_title = "Default Camera Sensor");

    /// Add a ChFilterRGBA8Access to the attached sensor
    /// Must be a camera
    void AddFilterRGBA8Access();

    /// Add a ChFilterVisualize to the attached sensor
    /// Must be a camera or lidar
    void AddFilterVisualize(unsigned int w = 1280,
    unsigned int h = 720);

    /// Add a ChFilterSave to the attached sensor
    /// Must be a camera or lidar
    void AddFilterSave(std::string file_path = "");

    /// Get sensor vis type
    SensorVisType GetSensorVisType() { return m_sensor_type; }

    /// Set the ChSensor
    void SetSensor(std::shared_ptr<ChSensor> sensor) { m_sensor = sensor; }

    /// Get the ChSensor
    std::shared_ptr<ChSensor> GetSensor() { return m_sensor; }

    /// Set the ChSensorManager
    void SetSensorManager(std::shared_ptr<ChSensorManager> manager) { m_sensor_manager = manager; }

    /// Get the ChSensorManager
    std::shared_ptr<ChSensorManager> GetSensorManager() { return m_sensor_manager; }

    /// Check if sensor manager has been initialized
    bool HasSensorManager() { return m_sensor_manager != nullptr; }

  private:
    SynSenVehicleVis::SensorVisType m_sensor_type;  ///< sensor vis type

    std::shared_ptr<ChSensor> m_sensor;  ///< handle to wrapped sensor

    std::shared_ptr<ChSensorManager> m_sensor_manager;  ///< handle to the sensor manager

    bool m_needs_reconstruction;  ///< need to reconstruct the scene?
};

}  // namespace visualization
}  // namespace synchrono

#endif  // SYN_SENSOR_VIS_H
