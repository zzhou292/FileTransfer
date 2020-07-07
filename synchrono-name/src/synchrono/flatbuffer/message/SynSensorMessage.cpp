#include "synchrono/flatbuffer/message/SynSensorMessage.h"

namespace synchrono {

/// Constructors
SynSensorMessage::SynSensorMessage(int id) : SynMessage(id, SynMessage::SENS) {}
SynSensorMessage::SynSensorMessage(int id, std::shared_ptr<State> state) : SynMessage(id, SynMessage::SENS) {
    m_state = std::static_pointer_cast<State>(state);
}
SynSensorMessage::SynSensorMessage(int id, const SynFlatBuffers::Message* message) : SynMessage(id, SynMessage::SENS) {
    GenerateState(message);
}

/// Generate state from FlatBuffers message
void SynSensorMessage::GenerateState(const SynFlatBuffers::Message* message) {
    /// System of casts from SynFlatBuffers::Message to SynFlatBuffers::Sensor::SensorBuffer
    const SynFlatBuffers::Sensor::State* state = message->message_as_Sensor_State();
    const SynFlatBuffers::Sensor::SensorBuffer* buffer = state->buffer();

    auto sensor_buffer = chrono_types::make_shared<SensorBuffer>(buffer->Width(), buffer->Height());
    // Buffer() returns flatbuffers::Vector, so Data() returns uint8_t*
    sensor_buffer->Buffer =
        std::vector<uint8_t>(buffer->Buffer()->Data(), buffer->Buffer()->Data() + buffer->Buffer()->size());

    m_state = chrono_types::make_shared<State>(0, (State::Type)state->type(), sensor_buffer);
}

/// Generate FlatBuffers message from this message's state
FlatBufferMessage SynSensorMessage::GenerateMessage(flatbuffers::FlatBufferBuilder& builder) {
    auto state = std::static_pointer_cast<State>(m_state);

    auto buffer = builder.CreateVector(state->buffer->Buffer);

    auto sensor_buffer =
        SynFlatBuffers::Sensor::CreateSensorBuffer(builder, state->buffer->Width, state->buffer->Height, buffer);

    auto flatbuffer_state = SynFlatBuffers::Sensor::CreateState(builder, sensor_buffer, state->GetTypeAsFlatBuffers());

    FlatBufferMessage message = flatbuffers::Offset<SynFlatBuffers::Message>(
        SynFlatBuffers::CreateMessage(builder, SynFlatBuffers::Type_Sensor_State, flatbuffer_state.Union(),
                                      builder.CreateString(std::to_string(m_id))));
    return message;
}

}  // namespace synchrono
