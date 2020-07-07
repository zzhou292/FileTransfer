#include "synchrono/flatbuffer/message/SynMAPMessage.h"

namespace synchrono {

SynMAPMessage::SynMAPMessage(int id) : SynMessage(id, SynMessage::MAP) {}
SynMAPMessage::SynMAPMessage(int id, std::shared_ptr<SynMessage::State> state) : SynMessage(id, SynMessage::MAP) {
    m_state = std::static_pointer_cast<State>(state);
}
SynMAPMessage::SynMAPMessage(int id, const SynFlatBuffers::Message* message) : SynMessage(id, SynMessage::MAP) {
    GenerateState(message);
}

void SynMAPMessage::GenerateState(const SynFlatBuffers::Message* message) {
    /// System of casts from SynFlatBuffers::Message to SynFlatBuffers::MAP::State
    const SynFlatBuffers::MAP::State* state = message->message_as_MAP_State();
    std::vector<Lane> lanes;
    for (auto lane : (*state->lanes())) {
        lanes.push_back(Lane({lane->width(),
                              {lane->stopPos()->x(), lane->stopPos()->y(), lane->stopPos()->z()},
                              {lane->offsetPos()->x(), lane->offsetPos()->y(), lane->offsetPos()->z()}}));
    }
    this->m_state = chrono_types::make_shared<State>(state->time(), state->sender(), state->intersection(), lanes);
}

SynMAPMessage::State SynMAPMessage::GenerateState(const SynFlatBuffers::MAP::State* state) {
    std::vector<Lane> lanes;
    for (auto lane : (*state->lanes())) {
        lanes.push_back(Lane({lane->width(),
                              {lane->stopPos()->x(), lane->stopPos()->y(), lane->stopPos()->z()},
                              {lane->offsetPos()->x(), lane->offsetPos()->y(), lane->offsetPos()->z()}}));
    }
    return State(state->time(), state->sender(), state->intersection(), lanes);
}

flatbuffers::Offset<SynFlatBuffers::MAP::State> SynMAPMessage::GenerateMessage(
    flatbuffers::FlatBufferBuilder& builder,
    std::shared_ptr<SynMAPMessage::State> state) {
    std::vector<flatbuffers::Offset<SynFlatBuffers::MAP::Lane>> lanes;
    for (Lane l : state->lanes) {
        lanes.push_back(SynFlatBuffers::MAP::CreateLane(
            builder, l.width, SynFlatBuffers::CreateVector(builder, l.stopPos.x(), l.stopPos.y(), l.stopPos.z()),
            SynFlatBuffers::CreateVector(builder, l.offsetPos.x(), l.offsetPos.y(), l.offsetPos.z())));
    }

    flatbuffers::Offset<SynFlatBuffers::MAP::State> flatbuffer_state = SynFlatBuffers::MAP::CreateState(
        builder, state->time, state->sender, state->intersection, builder.CreateVector(lanes));
    return flatbuffer_state;
}

FlatBufferMessage SynMAPMessage::GenerateMessage(flatbuffers::FlatBufferBuilder& builder) {
    std::shared_ptr<State> state = std::static_pointer_cast<State>(m_state);

    std::vector<flatbuffers::Offset<SynFlatBuffers::MAP::Lane>> lanes;
    for (Lane l : state->lanes) {
        lanes.push_back(SynFlatBuffers::MAP::CreateLane(
            builder, l.width, SynFlatBuffers::CreateVector(builder, l.stopPos.x(), l.stopPos.y(), l.stopPos.z()),
            SynFlatBuffers::CreateVector(builder, l.offsetPos.x(), l.offsetPos.y(), l.offsetPos.z())));
    }

    flatbuffers::Offset<SynFlatBuffers::MAP::State> flatbuffer_state = SynFlatBuffers::MAP::CreateState(
        builder, state->time, state->sender, state->intersection, builder.CreateVector(lanes));

    FlatBufferMessage message = flatbuffers::Offset<SynFlatBuffers::Message>(SynFlatBuffers::CreateMessage(
        builder, SynFlatBuffers::Type_MAP_State, flatbuffer_state.Union(), builder.CreateString(std::to_string(m_id))));
    return message;
}

}  // namespace synchrono
