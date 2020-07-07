#ifndef SYN_CITY_BUS_AGENT_H
#define SYN_CITY_BUS_AGENT_H

#include "synchrono/SynApi.h"

#include "synchrono/SynVehicleAgent.h"


#include "chrono/physics/ChMaterialSurface.h"
#include "chrono_models/vehicle/citybus/CityBus.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::vehicle::citybus;

namespace synchrono {
namespace agent {

class SYN_API SynCityBusAgent : public SynVehicleAgent {
  public:
    /// Construct a citybus agent with specified rank
    SynCityBusAgent(int rank, ChSystem* system = 0);

    /// Destructor
    ~SynCityBusAgent() {}

    /// Initialize vehicle with the specified location and orientation, using the passed Chrono system.
    virtual void InitializeVehicle(const ChCoordsys<>& pose);

    /// Get this agent's vehicle
    virtual ChVehicle& GetVehicle() { return m_citybus->GetVehicle(); }

  private:
    std::shared_ptr<CityBus> m_citybus;  ///< handle to the citybus vehicle
};
}  // namespace agent
}  // namespace synchrono
#endif
