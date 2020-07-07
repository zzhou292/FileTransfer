#ifndef SYN_SEDAN_AGENT_H
#define SYN_SEDAN_AGENT_H

#include "synchrono/SynApi.h"

#include "synchrono/SynVehicleAgent.h"

#include "chrono_models/vehicle/sedan/Sedan.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::vehicle::sedan;

namespace synchrono {
namespace agent {

class SYN_API SynSedanAgent : public SynVehicleAgent {
  public:
    /// Construct a sedan agent with specified rank
    SynSedanAgent(int rank, ChSystem* system = 0);

    /// Destructor
    ~SynSedanAgent() {}

    /// Initialize vehicle with the specified location and orientation, using the passed Chrono system.
    virtual void InitializeVehicle(const ChCoordsys<>& pose);

    /// Get this agent's vehicle
    virtual ChVehicle& GetVehicle() { return m_sedan->GetVehicle(); }

  private:
    std::shared_ptr<Sedan> m_sedan;  ///< handle to sedan vehicle
};
}  // namespace agent
}  // namespace synchrono
#endif
