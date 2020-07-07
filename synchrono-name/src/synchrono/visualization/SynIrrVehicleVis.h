#ifndef SYN_IRR_VEHICLE_VIS_H
#define SYN_IRR_VEHICLE_VIS_H

#include "synchrono/SynApi.h"

#include "synchrono/SynVis.h"

#include "chrono_vehicle/ChVehicle.h"
#include "chrono_vehicle/ChDriver.h"

#include "chrono_vehicle/wheeled_vehicle/utils/ChWheeledVehicleIrrApp.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::irrlicht;

namespace synchrono {
namespace visualization {

class SYN_API SynIrrVehicleVis : public SynVis {
  public:
    /// Constructs a wheeled vehicle irrlicht visualization
    SynIrrVehicleVis(std::shared_ptr<ChDriver> driver, double step_size, double render_step_size = 1. / 50);

    /// Destructor
    ~SynIrrVehicleVis() {}

    /// Initialze as default irrlicht wheeled vehicle app
    void InitializeAsDefaultChaseCamera(ChVehicle& vehicle, std::wstring window_title = L"");

    /// Allow external creation and attachment of a IrrApp
    void AttachIrrApp(std::shared_ptr<ChVehicleIrrApp> app);

    /// Get the handle to the irrlicht application
    std::shared_ptr<ChVehicleIrrApp> GetIrrApp() { return m_app; }

    /// Advance the state of this visualizer.
    virtual void Update(double step) override;

    /// Initialize the irrlicht visualizer
    virtual void Initialize() override {}

    /// Set the handle to the ChDriver
    void SetDriver(std::shared_ptr<ChDriver> driver) { m_driver = driver; }

    /// Set the save value
    void SetSave(bool save) { m_save = save; }

    /// Get the save value
    bool GetSave() { return m_save; }

    /// Set the visualize value
    void SetVisualize(bool visualize) { m_visualize = visualize; }

    /// Get the visualize value
    bool GetVisualize() { return m_visualize; }

  private:
    double m_step_size;      ///< simulation step size
    int m_step_number;       ///< times update has been called
    int m_render_step_size;  ///< time interval between two render frame [FPS]
    int m_render_steps;      ///< updates between each render frame

    bool m_visualize = true;  ///< if should visualize irrlicht window

    // Values for saving irrlicht window
    bool m_save = false;               ///< whether or not save the window to file
    std::string m_save_path = "img_";  ///< path to where these files should be saved
    int m_render_frame = 0;            ///< render frame number, used for file name when saving

    std::shared_ptr<ChVehicleIrrApp> m_app;  ///< handle to irrlicht app

    std::shared_ptr<ChDriver> m_driver;  ///< handle to vehicle driver
};

}  // namespace visualization
}  // namespace synchrono

#endif
