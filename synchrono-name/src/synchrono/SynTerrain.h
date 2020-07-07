#ifndef SYN_TERRAIN_H
#define SYN_TERRAIN_H

#include "synchrono/SynApi.h"

#include "synchrono/SynFlatBuffersManager.h"
#include "synchrono/flatbuffer/SynMessage.h"

#include "chrono_vehicle/ChTerrain.h"

using namespace chrono;
using namespace chrono::vehicle;

namespace synchrono {
namespace agent {

class SYN_API SynTerrain : public ChTerrain {
  public:
    SynTerrain(){};
    ~SynTerrain(){};

    virtual void Synchronize(double time) { GetTerrain()->Synchronize(time); }
    virtual void Advance(double step_size) { GetTerrain()->Advance(step_size); }

    virtual std::shared_ptr<ChTerrain> GetTerrain() const { return NULL; };

    virtual void AddMessage(SynFlatBuffersManager* manager){};
    virtual void SyncTerrain(SynMessage* message){};

    /// Generate SynMessage from SynFlatbuffers::Message
    virtual SynMessage* GenerateMessageFromFlatBuffers(const SynFlatBuffers::Message* message) { return nullptr; }

    /// Generate SynMessage to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) {}

    /// Get the terrain height at the specified (x,y) location.
    // virtual double GetHeight(const ChVector<double>& loc) = 0;

    /// Get the terrain normal at the specified (x,y) location.
    // virtual ChVector<> GetNormal(const ChVector<double>& loc) = 0;

    /// Get the terrain coefficient of friction at the specified (x,y) location.
    /// This coefficient of friction value may be used by certain tire models to modify
    /// the tire characteristics, but it will have no effect on the interaction of the terrain
    /// with other objects (including tire models that do not explicitly use it).
    // virtual float GetCoefficientFriction(const ChVector<double>& loc) = 0;
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYNTERRAIN_H
