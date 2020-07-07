#ifndef SYN_HMMWV_AGENT_H
#define SYN_HMMWV_AGENT_H

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

/// Reduced hmmwv model used by default
class SYN_API SynHMMWVAgent : public SynVehicleAgent {
  public:
    /// Construct a hmmwv agent with specified rank
    SynHMMWVAgent(int rank, ChSystem* system = 0);

    /// Destructor
    ~SynHMMWVAgent() {}

    /// Initialize vehicle with the specified location and orientation, using the passed Chrono system.
    virtual void InitializeVehicle(const ChCoordsys<>& pose) override;

    virtual void SetZombieTexture();

    virtual void SetUseLuggedTires(const bool use_lugged_tires) { m_use_lugged_tires = use_lugged_tires; }
    virtual bool GetUseLuggedTires() { return m_use_lugged_tires; }

    virtual std::shared_ptr<HMMWV_Reduced> GetHMMWV() { return m_hmmwv; }

    /// Get this agent's vehicle
    virtual ChVehicle& GetVehicle() { return m_hmmwv->GetVehicle(); }

    static void CreateLuggedGeometry(std::shared_ptr<ChBody> wheel_body,
                                     std::shared_ptr<ChMaterialSurfaceSMC> wheel_material);

  private:
    std::shared_ptr<HMMWV_Reduced> m_hmmwv;  ///< handle to hmmwv vehicle

    bool m_use_lugged_tires;  ///< Whether to use lugged tractor tires, good for some visualizations. Default = false
};
}  // namespace agent
}  // namespace synchrono
#endif
