#ifndef SYNRIGIDTERRAIN_H
#define SYNRIGIDTERRAIN_H

#include "chrono/assets/ChTriangleMeshShape.h"

#include "chrono/core/ChBezierCurve.h"

#include "chrono_vehicle/terrain/RigidTerrain.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/ChPart.h"

#include "synchrono/SynTerrain.h"

#ifdef SENSOR
#include "chrono/assets/ChVisualMaterial.h"
#endif  // SENSOR

using namespace chrono;
using namespace chrono::vehicle;

namespace synchrono {
namespace agent {

class SYN_API SynRigidTerrain : public SynTerrain {
  public:
    SynRigidTerrain(ChSystem* system, std::string collision_mesh_path, std::string vis_mesh_path);
    SynRigidTerrain(ChSystem* system, double terrainHeight, double terrainLength, double terrainWidth);
    SynRigidTerrain(ChSystem* system);
    // Jay - Use of ifdef here is only for compatibility with Chrono 5.0.1, not really related to SCM terrain...
#ifdef SYN_SCM
    SynRigidTerrain(ChSystem* system,
                    double terrainLength,
                    double terrainWidth,
                    std::shared_ptr<ChBezierCurve> path,
                    double height,
                    int width,
                    double dx,
                    double dy);
#endif
    ~SynRigidTerrain(){};

    void Initialize(std::shared_ptr<RigidTerrain::Patch> patch, std::shared_ptr<RigidTerrain> terrain);

    virtual ChVector<double> GetNormal(const ChVector<double>&) const override;
    virtual double GetHeight(const ChVector<double>& loc) const override;
    virtual float GetCoefficientFriction(const ChVector<double>& loc) const override;

    virtual std::shared_ptr<ChTerrain> GetTerrain() const override { return m_terrain; }
    virtual std::shared_ptr<RigidTerrain> GetRigidTerrain() const { return m_terrain; }

  private:
    std::shared_ptr<RigidTerrain> m_terrain;
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYNRIGIDTERRAIN_H
