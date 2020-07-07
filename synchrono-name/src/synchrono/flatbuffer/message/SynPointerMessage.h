#ifndef SYN_POINTER_MESSAGE_H
#define SYN_POINTER_MESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"

namespace synchrono {
class SYN_API SynPointerMessage : public SynMessage {
  public:
    /// Constructor
    SynPointerMessage(int id, uint8_t* buff, int bytes);

    /// Generate state from FlatBuffers message
    virtual void GenerateState(const SynFlatBuffers::Message* message) override;

    /// Generate FlatBuffers message from this message's state
    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override;

  private:
    uint8_t* m_buff;
    int m_bytes;
};
}  // namespace synchrono

#endif
