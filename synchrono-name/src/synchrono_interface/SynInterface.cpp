#include "synchrono_interface/SynInterface.h"

namespace synchrono {
namespace interface {
SynInterface::SynInterface(SynFlatBuffersManager& flatbuffers_manager, std::string port)
    : m_flatbuffers_manager(flatbuffers_manager), m_transport_manager(port) {}

void SynInterface::Synchronize(double ch_time) {
    for (auto component : m_components) {
        component.second->Advance(ch_time);
        if (component.second->GetType() == SynComponent::SENDER) {
            if (component.second->ShouldUpdate(ch_time)) {
                m_flatbuffers_manager.AddMessage(
                    component.second->GetMessage()->GenerateMessage(m_flatbuffers_manager.GetBuilder()));
            }
        }
    }

    m_transport_manager.Send(m_flatbuffers_manager);
    m_transport_manager.Receive(m_flatbuffers_manager);

    for (auto component : m_components) {
        if (component.second->GetType() == SynComponent::RECEIVER) {
            if (auto msg = m_flatbuffers_manager.Get(component.second->GetID())) {
                component.second->GetMessage()->GenerateState(msg);
            }
        }
    }

    m_flatbuffers_manager.Reset();
}

void SynInterface::AddComponent(std::shared_ptr<SynComponent> component) {
    auto it = m_components.find(component->GetID());
    if (it != m_components.end()) {
        throw "Each component must have a unique name.";
    }
    m_components.insert(std::pair<std::string, std::shared_ptr<SynComponent>>(component->GetID(), component));
}

}  // namespace interface
}  // namespace synchrono
