#ifndef SYN_VEHICLE_MODULE_H
#define SYN_VEHICLE_MODULE_H

#include "synchrono/SynApi.h"

#include "synchrono/SynComponent.h"

#include "synchrono/flatbuffer/SynMessage.h"

#include "chrono_vehicle/ChDriver.h"

using namespace chrono::vehicle;

namespace synchrono {
namespace agent {

class SYN_API SynVehicleComponent : public SynComponent {
  public:
    /// Constructor which takes and sets this brains ChDriver and ChVehicle
    SynVehicleComponent(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
        : SynComponent(rank), m_driver(driver), m_vehicle(vehicle) {}

    /// Destructor
    ~SynVehicleComponent() {}

    /// Advance the state of this brain until brain time syncs with passed time
    virtual void Advance(double step)=0;

    /// Synchronize this brain to the specified time
    virtual void Synchronize(double time)=0;

    /// Process an incoming message
    virtual void ProcessMessage(SynMessage* msg, int sender_rank) =0;

    /// Get the driver inputs from the attached driver
    ChDriver::Inputs GetDriverInputs() { return m_driver->GetInputs(); }

    /// Get the attached driver
    std::shared_ptr<ChDriver> GetDriver() { return m_driver; }

  protected:
    std::shared_ptr<ChDriver> m_driver;  ///< handle to the ChDriver
    ChVehicle& m_vehicle;                ///< reference to the ChVehicle
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYN_VEHICLE_BRAIN_H
