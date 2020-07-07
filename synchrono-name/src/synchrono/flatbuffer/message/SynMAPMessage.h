#ifndef SYN_MAP_MESSAGE_H
#define SYN_MAP_MESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"

namespace synchrono {

class SYN_API SynMAPMessage : public SynMessage {
  public:
    struct Lane {
        double width;
        ChVector<> stopPos;
        ChVector<> offsetPos;
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
    SynMAPMessage(int id);
    SynMAPMessage(int id, std::shared_ptr<SynMessage::State> state);
    SynMAPMessage(int id, const SynFlatBuffers::Message* message);

    /// Generate state from FlatBuffers message
    virtual void GenerateState(const SynFlatBuffers::Message* message) override;

    /// Generate FlatBuffers message from this message's state
    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override;

    State GenerateState(const SynFlatBuffers::MAP::State* state);

    flatbuffers::Offset<SynFlatBuffers::MAP::State> GenerateMessage(flatbuffers::FlatBufferBuilder& builder,
                                                                    std::shared_ptr<SynMAPMessage::State> state);
};
}  // namespace synchrono

#endif
