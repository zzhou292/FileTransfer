#ifndef SYN_SPAT_MESSAGE_H
#define SYN_SPAT_MESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"

namespace synchrono {
class SYN_API SynSPATMessage : public SynMessage {
  public:
    enum Color { GREEN, YELLOW, RED };

    struct Lane {
        Color color;
    };

    struct State : SynMessage::State {
        double sender;
        double intersection;
        std::vector<Lane> lanes;

        State() : SynMessage::State(0.0) {}

        State(double time, double sender, double intersection, std::vector<Lane> lanes)
            : SynMessage::State(time), sender(sender), intersection(intersection), lanes(lanes) {}
    };

    /// Constructors
    SynSPATMessage(int id);
    SynSPATMessage(int id, std::shared_ptr<SynMessage::State> state);
    SynSPATMessage(int id, const SynFlatBuffers::Message* message);

    /// Generate state from FlatBuffers message
    virtual void GenerateState(const SynFlatBuffers::Message* message) override;

    /// Generate FlatBuffers message from this message's state
    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override;

    State GenerateState(const SynFlatBuffers::SPAT::State* state);

    flatbuffers::Offset<SynFlatBuffers::SPAT::State> GenerateMessage(flatbuffers::FlatBufferBuilder& builder,
                                                                     std::shared_ptr<SynSPATMessage::State> state);

  private:
};
}  // namespace synchrono

#endif
