
#include "synchrono/flatbuffer/message/SynSPATMessage.h"

namespace synchrono {

SynSPATMessage::SynSPATMessage(int id) : SynMessage(id, SynMessage::SPAT) {}
SynSPATMessage::SynSPATMessage(int id, std::shared_ptr<SynMessage::State> state) : SynMessage(id, SynMessage::SPAT) {
    m_state = std::static_pointer_cast<State>(state);
}
SynSPATMessage::SynSPATMessage(int id, const SynFlatBuffers::Message* message) : SynMessage(id, SynMessage::SPAT) {
    GenerateState(message);
}

void SynSPATMessage::GenerateState(const SynFlatBuffers::Message* message) {
    /// System of casts from SynFlatBuffers::Message to SynFlatBuffers::SPAT::State
    const SynFlatBuffers::SPAT::State* state = message->message_as_SPAT_State();

    std::vector<Lane> lanes;
    for (auto lane : (*state->lanes())) {
        lanes.push_back(Lane({static_cast<Color>(lane->color())}));
    }
    m_state = chrono_types::make_shared<State>(state->time(), state->sender(), state->intersection(), lanes);
}

SynSPATMessage::State SynSPATMessage::GenerateState(const SynFlatBuffers::SPAT::State* state) {
    std::vector<Lane> lanes;
    for (auto lane : (*state->lanes())) {
        lanes.push_back(Lane({static_cast<Color>(lane->color())}));
    }

    return State(state->time(), state->sender(), state->intersection(), lanes);
}

flatbuffers::Offset<SynFlatBuffers::SPAT::State> SynSPATMessage::GenerateMessage(
    flatbuffers::FlatBufferBuilder& builder,
    std::shared_ptr<SynSPATMessage::State> state) {
    std::vector<flatbuffers::Offset<SynFlatBuffers::SPAT::Lane>> lanes;
    for (Lane lane : state->lanes) {
        auto color = SynFlatBuffers::SPAT::Color(lane.color);
        lanes.push_back(SynFlatBuffers::SPAT::CreateLane(builder, static_cast<SynFlatBuffers::SPAT::Color>(color)));
    }

    flatbuffers::Offset<SynFlatBuffers::SPAT::State> flatbuffer_state = SynFlatBuffers::SPAT::CreateState(
        builder, state->time, state->sender, state->intersection, builder.CreateVector(lanes));
    return flatbuffer_state;
}

FlatBufferMessage SynSPATMessage::GenerateMessage(flatbuffers::FlatBufferBuilder& builder) {
    std::shared_ptr<State> state = std::static_pointer_cast<State>(m_state);

    std::vector<flatbuffers::Offset<SynFlatBuffers::SPAT::Lane>> lanes;
    for (Lane lane : state->lanes) {
        auto color = SynFlatBuffers::SPAT::Color(lane.color);
        lanes.push_back(SynFlatBuffers::SPAT::CreateLane(builder, static_cast<SynFlatBuffers::SPAT::Color>(color)));
    }

    flatbuffers::Offset<SynFlatBuffers::SPAT::State> flatbuffer_state = SynFlatBuffers::SPAT::CreateState(
        builder, state->time, state->sender, state->intersection, builder.CreateVector(lanes));

    FlatBufferMessage message = flatbuffers::Offset<SynFlatBuffers::Message>(
        SynFlatBuffers::CreateMessage(builder, SynFlatBuffers::Type_SPAT_State, flatbuffer_state.Union(),
                                      builder.CreateString(std::to_string(m_id))));
    return message;
}

}  // namespace synchrono
