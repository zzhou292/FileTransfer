#ifndef SYN_VIS_MANAGER_H
#define SYN_VIS_MANAGER_H

#include "synchrono/SynApi.h"

#include "synchrono/SynVis.h"

#include "chrono/physics/ChSystem.h"

using namespace chrono;

namespace synchrono {
namespace visualization {

class SYN_API SynVisManager {
  public:
    /// Constructs a visualization manager
    SynVisManager() {}

    /// Destructor
    ~SynVisManager() {}

    /// Initializes visualization manager
    void Update(double step);

    /// Attaches specified visualizer to manager
    void AttachVis(std::shared_ptr<SynVis> vis);

    /// Get visualization list
    std::vector<std::shared_ptr<SynVis>> GetVisList() { return m_vis_list; }

  protected:
    std::vector<std::shared_ptr<SynVis>> m_vis_list;  ///< vector of handles to visualizations
};

}  // namespace visualization
}  // namespace synchrono

#endif  // SYN_VIS_MANAGER_H
