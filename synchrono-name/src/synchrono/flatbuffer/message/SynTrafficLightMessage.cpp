#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

namespace synchrono {

SynTrafficLightMessage::SynTrafficLightMessage(int id)
    : SynMessage(id, SynMessage::TRAFFIC_LIGHT),
      m_map_msg(chrono_types::make_shared<SynMAPMessage>(id)),
      m_spat_msg(chrono_types::make_shared<SynSPATMessage>(id)) {
    m_state = chrono_types::make_shared<State>(State());
}

SynTrafficLightMessage::SynTrafficLightMessage(int id, std::shared_ptr<SynMessage::State> state)
    : SynMessage(id, SynMessage::TRAFFIC_LIGHT) {
    m_state = std::static_pointer_cast<State>(state);
    m_map_msg = chrono_types::make_shared<SynMAPMessage>(
        id, chrono_types::make_shared<SynMAPMessage::State>(std::static_pointer_cast<State>(m_state)->map));
    m_spat_msg = chrono_types::make_shared<SynSPATMessage>(
        id, chrono_types::make_shared<SynSPATMessage::State>(std::static_pointer_cast<State>(m_state)->spat));
}

SynTrafficLightMessage::SynTrafficLightMessage(int id, const SynFlatBuffers::Message* message)
    : SynMessage(id, SynMessage::TRAFFIC_LIGHT) {
    GenerateState(message);
    m_map_msg = chrono_types::make_shared<SynMAPMessage>(
        id, chrono_types::make_shared<SynMAPMessage::State>(std::static_pointer_cast<State>(m_state)->map));
    m_spat_msg = chrono_types::make_shared<SynSPATMessage>(
        id, chrono_types::make_shared<SynSPATMessage::State>(std::static_pointer_cast<State>(m_state)->spat));
}

void SynTrafficLightMessage::GenerateState(const SynFlatBuffers::Message* message) {
    /// System of casts from SynFlatBuffers::Message to SynFlatBuffers::Agent::TrafficLight::State
    const SynFlatBuffers::Agent::State* agent_state = message->message_as_Agent_State();
    const SynFlatBuffers::Agent::TrafficLight::State* state = agent_state->message_as_TrafficLight_State();

    m_state = chrono_types::make_shared<State>(state->time(), m_map_msg->GenerateState(state->map()),
                                               m_spat_msg->GenerateState(state->spat()));
}

FlatBufferMessage SynTrafficLightMessage::GenerateMessage(flatbuffers::FlatBufferBuilder& builder) {
    std::shared_ptr<State> state = std::static_pointer_cast<State>(m_state);

    flatbuffers::Offset<SynFlatBuffers::Agent::State> flatbuffer_state = SynFlatBuffers::Agent::CreateState(
        builder, SynFlatBuffers::Agent::Type_TrafficLight_State,
        SynFlatBuffers::Agent::TrafficLight::CreateState(
            builder, state->time,
            m_map_msg->GenerateMessage(builder, chrono_types::make_shared<SynMAPMessage::State>(
                                                    std::static_pointer_cast<State>(m_state)->map)),
            m_spat_msg->GenerateMessage(builder, chrono_types::make_shared<SynSPATMessage::State>(
                                                     std::static_pointer_cast<State>(m_state)->spat)))
            .Union());

    FlatBufferMessage message = flatbuffers::Offset<SynFlatBuffers::Message>(
        SynFlatBuffers::CreateMessage(builder, SynFlatBuffers::Type_Agent_State, flatbuffer_state.Union(),
                                      builder.CreateString(std::to_string(m_id))));
    return message;
}

}  // namespace synchrono
