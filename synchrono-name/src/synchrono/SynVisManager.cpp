#include "synchrono/SynVisManager.h"

namespace synchrono {
namespace visualization {
void SynVisManager::Update(double step) {
    if (!m_vis_list.size())
        return;

    for (std::shared_ptr<SynVis> vis : m_vis_list)
        vis->Update(step);
}

void SynVisManager::AttachVis(std::shared_ptr<SynVis> vis) {
    vis->Initialize();
    m_vis_list.push_back(vis);
}

}  // namespace visualization
}  // namespace synchrono
