#ifndef SYN_FULLHMMWVAGENT_H
#define SYN_FULLHMMWVAGENT_H

#include "synchrono/SynApi.h"

#include "synchrono/SynVehicleAgent.h"

#include "chrono_models/vehicle/hmmwv/HMMWV.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono/assets/ChColorAsset.h"
#include "chrono/collision/ChConvexDecomposition.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::vehicle::hmmwv;

namespace synchrono {
namespace agent {

class SYN_API SynFullHMMWVAgent : public SynVehicleAgent {
  public:
    /// Construct a hmmwv agent with specified rank
    SynFullHMMWVAgent(int rank, ChSystem* system = 0);

    /// Destructor
    ~SynFullHMMWVAgent() {}

    /// Initialize vehicle with the specified location and orientation, using the passed Chrono system.
    virtual void InitializeVehicle(const ChCoordsys<>& pose) override;

    virtual void SetZombieTexture();

    virtual void SetUseLuggedTires(const bool lt) { m_use_lugged_tires = lt; }
    virtual bool GetUseLuggedTires() { return m_use_lugged_tires; }

    virtual std::shared_ptr<HMMWV_Full> GetHMMWV() { return m_hmmwv; }

    /// Get this agent's vehicle
    virtual ChVehicle& GetVehicle() { return m_hmmwv->GetVehicle(); }

  private:
    std::shared_ptr<HMMWV_Full> m_hmmwv;  ///< handle to hmmwv vehicle

    bool m_use_lugged_tires;  ///< Whether to use lugged tractor tires, good for some visualizations. Default = false
};
}  // namespace agent
}  // namespace synchrono

#endif  // SYN_FULLHMMWVAGENT_H
