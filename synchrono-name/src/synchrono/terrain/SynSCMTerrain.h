#ifndef SYNSCMTERRAIN_H
#define SYNSCMTERRAIN_H

#include "chrono/core/ChBezierCurve.h"
#include "chrono/physics/ChSystem.h"
#include "chrono_vehicle/terrain/SCMDeformableTerrain.h"

#include "synchrono/SynTerrain.h"
#include "synchrono/flatbuffer/message/SynSCMMessage.h"

using namespace chrono;

namespace synchrono {
namespace agent {

/// Defines SCM parameters for a custom constructor
/// Also defines default parameter initializations
struct SCMParameters {
    double m_Bekker_Kphi;    // Kphi, frictional modulus in Bekker model
    double m_Bekker_Kc;      // Kc, cohesive modulus in Bekker model
    double m_Bekker_n;       // n, exponent of sinkage in Bekker model (usually 0.6...1.8)
    double m_Mohr_cohesion;  // Cohesion in, Pa, for shear failure
    double m_Mohr_friction;  // Friction angle (in degrees!), for shear failure
    double m_Janosi_shear;   // J , shear parameter, in meters, in Janosi-Hanamoto formula (usually few mm or cm)
    double m_elastic_K;      // elastic stiffness K (must be > Kphi; very high values gives the original SCM model)
    double m_damping_R;  // vertical damping R, per unit area (vertical speed proportional, it is zero in original SCM
                         // model)

    // Set the parameters
    void SetParameters(double Bekker_Kphi,
                       double Bekker_Kc,
                       double Bekker_n,
                       double Mohr_cohesion,
                       double Mohr_friction,
                       double Janosi_shear,
                       double elastic_K,
                       double damping_R) {
        m_Bekker_Kphi = Bekker_Kphi;
        m_Bekker_Kc = Bekker_Kc;
        m_Bekker_n = Bekker_n;
        m_Mohr_cohesion = Mohr_cohesion;
        m_Mohr_friction = Mohr_friction;
        m_Janosi_shear = Janosi_shear;
        m_elastic_K = ChMax(elastic_K, Bekker_Kphi);
        m_damping_R = damping_R;
    }

    /// Soft default parameters
    void InitializeParametersAsSoft() {
        m_Bekker_Kphi = 0.2e6;
        m_Bekker_Kc = 0;
        m_Bekker_n = 1.1;
        m_Mohr_cohesion = 0;
        m_Mohr_friction = 30;
        m_Janosi_shear = 0.01;
        m_elastic_K = 4e7;
        m_damping_R = 3e4;
    }

    /// Middle default parameters
    void InitializeParametersAsMid() {
        m_Bekker_Kphi = 2e6;
        m_Bekker_Kc = 0;
        m_Bekker_n = 1.1;
        m_Mohr_cohesion = 0;
        m_Mohr_friction = 30;
        m_Janosi_shear = 0.01;
        m_elastic_K = 2e8;
        m_damping_R = 3e4;
    }

    /// Hard default parameters
    void InitializeParametersAsHard() {
        m_Bekker_Kphi = 5301e3;
        m_Bekker_Kc = 102e3;
        m_Bekker_n = 0.793;
        m_Mohr_cohesion = 1.3e3;
        m_Mohr_friction = 31.1;
        m_Janosi_shear = 1.2e-2;
        m_elastic_K = 4e8;
        m_damping_R = 3e4;
    }
};

class SYN_API SynSCMTerrain : public SynTerrain {
  public:
    /// Constructor
    /// Initialize the SCM terrain system as flat.
    SynSCMTerrain(ChSystem* system,
                  double sizeX = 10,              ///< [in] terrain dimension in the X direction
                  double sizeY = 10,              ///< [in] terrain dimension in the Y direction
                  double divX = 50,               ///< [in] number of divisions in the X direction
                  double divY = 50,               ///< [in] number of divisions in the Y direction
                  SCMParameters* params = NULL);  ///< [in] struct which holds the SCM parameters
    /// Initialize the SCM terrain system with a path
    SynSCMTerrain(ChSystem* system,
                  double sizeX,
                  double sizeY,
                  std::shared_ptr<ChBezierCurve> path,
                  double width,
                  double dx,
                  double dy,
                  SCMParameters* params = NULL);
    ~SynSCMTerrain(){};

    virtual void AddMessage(SynFlatBuffersManager* manager) override;
    virtual std::shared_ptr<ChTerrain> GetTerrain() const override { return m_terrain; }
    virtual std::shared_ptr<SCMDeformableTerrain> GetSCMTerrain() const { return m_terrain; }

    virtual void SyncTerrain(SynMessage* message) override;
    virtual ChVector<double> GetNormal(const ChVector<double>&) const override;
    virtual double GetHeight(const ChVector<double>& loc) const override;
    virtual float GetCoefficientFriction(const ChVector<double>& loc) const override;

    /// Generate SynMessage from SynFlatbuffers::Message
    virtual SynMessage* GenerateMessageFromFlatBuffers(const SynFlatBuffers::Message* message) override;

    /// Generate SynMessage to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) override;

  private:
    ChSystem* m_system;  ///< [in] pointer to the containing multibody system

    std::shared_ptr<SCMDeformableTerrain> m_terrain;

    std::vector<std::pair<int, chrono::Vector>> ConvertMapToVector(std::unordered_map<int, chrono::Vector> source_map);
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYNSCMTERRAIN_H
