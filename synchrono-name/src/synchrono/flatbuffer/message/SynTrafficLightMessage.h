#ifndef SYN_TRAFFIC_LIGHT_MESSAGE_H
#define SYN_TRAFFIC_LIGHT_MESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"
#include "synchrono/flatbuffer/message/SynSPATMessage.h"
#include "synchrono/flatbuffer/message/SynMAPMessage.h"

namespace synchrono {
class SYN_API SynTrafficLightMessage : public SynMessage {
  public:
    struct State : SynMessage::State {
        SynMAPMessage::State map;
        SynSPATMessage::State spat;

        State() : SynMessage::State(0.0), map(SynMAPMessage::State()), spat(SynSPATMessage::State()) {}

        State(double time, SynMAPMessage::State map, SynSPATMessage::State spat)
            : SynMessage::State(time), map(map), spat(spat) {}
    };

    /// Constructors
    SynTrafficLightMessage(int id);
    SynTrafficLightMessage(int id, std::shared_ptr<SynMessage::State> state);
    SynTrafficLightMessage(int id, const SynFlatBuffers::Message* message);

    /// Generate state from FlatBuffers message
    virtual void GenerateState(const SynFlatBuffers::Message* message) override;

    /// Generate FlatBuffers message from this message's state
    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override;

  private:
    std::shared_ptr<SynSPATMessage> m_spat_msg;  ///< Handle to the traffic light's spat message
    std::shared_ptr<SynMAPMessage> m_map_msg;    ///< Handle to the traffic light's map message
};
}  // namespace synchrono

#endif
