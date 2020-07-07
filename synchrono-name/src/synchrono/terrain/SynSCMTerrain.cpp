#include "synchrono/terrain/SynSCMTerrain.h"

namespace synchrono {
namespace agent {

SynSCMTerrain::SynSCMTerrain(ChSystem* system,
                             double sizeX,
                             double sizeY,
                             double divX,
                             double divY,
                             SCMParameters* params) {
    bool auto_refine = false;
    bool bulldoze = true;

    auto terrain = chrono_types::make_shared<vehicle::SCMDeformableTerrain>(system);

    terrain->Initialize(0, sizeX, sizeY, divX, divY);

    if (params != NULL) {
        terrain->SetSoilParameters(
            params->m_Bekker_Kphi,    // Bekker Kphi
            params->m_Bekker_Kc,      // Bekker Kc
            params->m_Bekker_n,       // Bekker n exponent
            params->m_Mohr_cohesion,  // Mohr cohesive limit (Pa)
            params->m_Mohr_friction,  // Mohr friction limit (degrees)
            params->m_Janosi_shear,   // Janosi shear coefficient (m)
            params->m_elastic_K,      // Elastic stiffness (Pa/m), before plastic yield, must be > Kphi
            params->m_damping_R);     // Damping (Pa s/m), proportional to negative vertical speed (optional)
    } else {
        terrain->SetSoilParameters(2e6,   // Bekker Kphi
                                   0,     // Bekker Kc
                                   1.1,   // Bekker n exponent
                                   0,     // Mohr cohesive limit (Pa)
                                   30,    // Mohr friction limit (degrees)
                                   0.01,  // Janosi shear coefficient (m)
                                   40e7,  // Elastic stiffness (Pa/m), before plastic yield, must be > Kphi
                                   3e4);  // Damping (Pa s/m), proportional to negative vertical speed (optional)
    }

    terrain->SetBulldozingFlow(bulldoze);
    if (bulldoze) {
        terrain->SetBulldozingParameters(
            // 55
            55,   // angle of friction for erosion of displaced material at the border of the rut
            1,    // displaced material vs downward pressed material.
            5,    // number of erosion refinements per timestep
            10);  // number of concentric vertex selections subject to erosion
    }

    // Turn on the automatic level of detail refinement, so a coarse terrain mesh
    // is automatically improved by adding more points under the wheel contact patch:
    terrain->SetAutomaticRefinement(auto_refine);
    if (auto_refine) {
        terrain->SetAutomaticRefinementResolution(0.3);
    }

    // i.e. is SynChrono actually syncing this terrain
    terrain->SetMeshSyncing(true);

    terrain->SetPlotType(vehicle::SCMDeformableTerrain::PLOT_SINKAGE, 0, 0.1);
    terrain->GetMesh()->SetWireframe(true);

    m_terrain = terrain;
}

SynSCMTerrain::SynSCMTerrain(ChSystem* system,
                             double sizeX,
                             double sizeY,
                             std::shared_ptr<ChBezierCurve> path,
                             double width,
                             double dx,
                             double dy,
                             SCMParameters* params) {
    bool auto_refine = false;
    bool bulldoze = false;

    auto terrain = chrono_types::make_shared<vehicle::SCMDeformableTerrain>(system);

    terrain->Initialize(path, "", 0, width, dx, dy);

    if (params != NULL) {
        terrain->SetSoilParameters(
            params->m_Bekker_Kphi,    // Bekker Kphi
            params->m_Bekker_Kc,      // Bekker Kc
            params->m_Bekker_n,       // Bekker n exponent
            params->m_Mohr_cohesion,  // Mohr cohesive limit (Pa)
            params->m_Mohr_friction,  // Mohr friction limit (degrees)
            params->m_Janosi_shear,   // Janosi shear coefficient (m)
            params->m_elastic_K,      // Elastic stiffness (Pa/m), before plastic yield, must be > Kphi
            params->m_damping_R);     // Damping (Pa s/m), proportional to negative vertical speed (optional)
    } else {
        terrain->SetSoilParameters(2e6,   // Bekker Kphi
                                   0,     // Bekker Kc
                                   1.1,   // Bekker n exponent
                                   0,     // Mohr cohesive limit (Pa)
                                   30,    // Mohr friction limit (degrees)
                                   0.01,  // Janosi shear coefficient (m)
                                   40e7,  // Elastic stiffness (Pa/m), before plastic yield, must be > Kphi
                                   3e4);  // Damping (Pa s/m), proportional to negative vertical speed (optional)
    }

    // Hard parameters
    // terrain->SetSoilParameters(5301e3, 102e3, 0.793, 1.3e3, 31.1, 2e-2, 4e8, 3e4);

    terrain->SetBulldozingFlow(bulldoze);
    if (bulldoze) {
        terrain->SetBulldozingParameters(
            55,   // angle of friction for erosion of displaced material at the border of the rut
            1,    // displaced material vs downward pressed material.
            5,    // number of erosion refinements per timestep
            10);  // number of concentric vertex selections subject to erosion
    }

    // Turn on the automatic level of detail refinement, so a coarse terrain mesh
    // is automatically improved by adding more points under the wheel contact patch:
    terrain->SetAutomaticRefinement(auto_refine);
    if (auto_refine) {
        terrain->SetAutomaticRefinementResolution(0.3);
    }

    // i.e. is SynChrono actually syncing this terrain
    terrain->SetMeshSyncing(true);

    terrain->SetPlotType(vehicle::SCMDeformableTerrain::PLOT_SINKAGE, 0, 0.1);
    terrain->GetMesh()->SetWireframe(true);

    m_terrain = terrain;
}

ChVector<double> SynSCMTerrain::GetNormal(const ChVector<double>& loc) const {
    return GetSCMTerrain()->GetNormal(loc);
}

double SynSCMTerrain::GetHeight(const ChVector<double>& loc) const {
    return GetSCMTerrain()->GetHeight(loc);
}

float SynSCMTerrain::GetCoefficientFriction(const ChVector<double>& loc) const {
    return GetSCMTerrain()->GetCoefficientFriction(loc);
}

void SynSCMTerrain::AddMessage(SynFlatBuffersManager* manager) {
    auto state = chrono_types::make_shared<SynSCMMessage::State>(ConvertMapToVector(m_terrain->getMeshDiffs()));

    SynSCMMessage* msg = new SynSCMMessage(0, state);
    manager->AddMessage(msg);
    delete msg;
}

void SynSCMTerrain::GenerateMessagesToSend(std::vector<SynMessage*>& messages) {
    auto state = chrono_types::make_shared<SynSCMMessage::State>(ConvertMapToVector(m_terrain->getMeshDiffs()));

    messages.push_back(new SynSCMMessage(0, state));
}

std::vector<std::pair<int, chrono::Vector>> SynSCMTerrain::ConvertMapToVector(
    std::unordered_map<int, chrono::Vector> source_map) {
    std::vector<std::pair<int, chrono::Vector>> dest_vec;
    dest_vec.insert(dest_vec.end(), source_map.begin(), source_map.end());
    return dest_vec;
}

void SynSCMTerrain::SyncTerrain(SynMessage* msg) {
    std::shared_ptr<SynSCMMessage::State> state = std::static_pointer_cast<SynSCMMessage::State>(msg->GetState());
    m_terrain->applyMeshDiffs(state->mesh_diffs);
}

/// Generate SynMessage from SynFlatbuffers::Message
SynMessage* SynSCMTerrain::GenerateMessageFromFlatBuffers(const SynFlatBuffers::Message* message) {
    return new SynSCMMessage(0, message);
}

}  // namespace agent
}  // namespace synchrono
