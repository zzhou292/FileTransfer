#include "synchrono/flatbuffer/message/SynPointerMessage.h"

namespace synchrono {

SynPointerMessage::SynPointerMessage(int id, uint8_t* buff, int bytes)
    : SynMessage(id, SynMessage::POINTER), m_buff(buff), m_bytes(bytes) {}

void SynPointerMessage::GenerateState(const SynFlatBuffers::Message* message) {}

FlatBufferMessage SynPointerMessage::GenerateMessage(flatbuffers::FlatBufferBuilder& builder) {
    flatbuffers::Offset<SynFlatBuffers::Pointer> pointer =
        SynFlatBuffers::CreatePointer(builder, builder.CreateVector<unsigned char>(m_buff, m_bytes));

    FlatBufferMessage message = flatbuffers::Offset<SynFlatBuffers::Message>(
        SynFlatBuffers::CreateMessage(builder, SynFlatBuffers::Type_Pointer, pointer.Union()));

    return message;
}

}  // namespace synchrono
