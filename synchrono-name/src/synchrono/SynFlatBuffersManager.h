#ifndef SYN_FLATBUFFERS_MANAGER_H
#define SYN_FLATBUFFERS_MANAGER_H

#include <vector>

#include "synchrono/SynApi.h"
#include "synchrono/flatbuffer/SynMessage.h"
#include "synchrono/flatbuffer/SynFlatBuffers_generated.h"

/// Manager that handles all flatbuffer specifics
namespace synchrono {
class SYN_API SynFlatBuffersManager {
  public:
    /// Construct a flatbuffers manager
    SynFlatBuffersManager() {}

    /// Destructor
    ~SynFlatBuffersManager() {}

    /// Adds a SynMessage to the flatbuffer message buffer
    void AddMessage(SynMessage* message);

    /// Adds a SynFlatBuffers Message to the flatbuffer message buffer
    void AddMessage(flatbuffers::Offset<SynFlatBuffers::Message> message);

    /// Completes the flatbuffer message. Creates a buffer message, of which stores every message
    void Finish();

    /// Completes the flatbuffer message with a 4 bytes in the front of the buffer, which has the size of the byte array
    /// Creates a buffer message, of which stores every message
    void FinishSizePrefixed();

    /// Reset the flatbuffer. Must be called, otherwise messages will just continue to be added to the vector (memory
    /// leak).
    void Reset();

    /// When a message is received through MPI, the byte array (buffer) is stored in the class variable m_buffer. This
    /// buffer is then converted to a flatbuffer message. The ith message is then retrieved from this buffer message.
    const SynFlatBuffers::Message* Get(int i);

    /// When a message is received through TCP (interface), the byte array (buffer) is stored in the class variable
    /// m_buffer. This buffer is then converted to a flatbuffer message. The message with the same id is then retrieved
    /// from this buffer message.
    const SynFlatBuffers::Message* Get(std::string id);

    /// Gets the size of the buffer message
    int32_t GetSize() { return m_builder.GetSize(); }

    /// Gets the buffer pointer
    uint8_t* GetBufferPointer() { return m_builder.GetBufferPointer(); }

    /// Gets the underlying buffer
    std::vector<uint8_t>& GetBuffer() { return m_buffer; }

    /// Gets the FlatBufferBuilder associated with this manager
    flatbuffers::FlatBufferBuilder& GetBuilder() { return m_builder; }

  private:
    flatbuffers::FlatBufferBuilder m_builder;  ///< FlatBufferBuilder associated with this manager

    std::vector<flatbuffers::Offset<SynFlatBuffers::Message>> m_message_vector;  ///< vector of SynFlatBuffers messages

    std::vector<uint8_t> m_buffer;  ///< When a message is received, the byte array is stored in this buffer
};
}  // namespace synchrono

#endif
