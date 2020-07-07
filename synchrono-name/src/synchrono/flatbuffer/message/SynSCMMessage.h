#ifndef SYN_SCMMESSAGE_H
#define SYN_SCMMESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"
#include "chrono_vehicle/terrain/SCMDeformableTerrain.h"

namespace synchrono {

class SYN_API SynSCMMessage : public SynMessage {
  public:
    struct State : SynMessage::State {
        // SynMessage::Pose chassis;
        std::vector<std::pair<int, chrono::Vector>> mesh_diffs;

        State() : SynMessage::State(0.0) {}

        State(double time) : SynMessage::State(time) {}
        State(std::vector<std::pair<int, chrono::Vector>> diffs) { mesh_diffs = diffs; }
    };

    SynSCMMessage(int id, SynMessage::Type type) : SynMessage(id, type){};

    SynSCMMessage(int id, std::shared_ptr<SynSCMMessage::State> state) : SynMessage(id, SynMessage::TERRAIN) {
        m_state = std::static_pointer_cast<SynMessage::State>(state);
    };

    SynSCMMessage(int id, const SynFlatBuffers::Message* message) : SynMessage(id, SynMessage::TERRAIN) {
        GenerateState(message);
    }

    static std::shared_ptr<SynSCMMessage::State> GenerateStateFromMessage(const SynFlatBuffers::Message* message) {
        const SynFlatBuffers::SCM::State* terrain_state = message->message_as_SCM_State();

        std::vector<std::pair<int, chrono::Vector>> diffs;
        auto diffs_len = terrain_state->diffs()->size();
        for (int i = 0; i < diffs_len; i++) {
            const SynFlatBuffers::SCM::Node* node = terrain_state->diffs()->Get(i);
            chrono::Vector vec(node->diff()->x(), node->diff()->y(), node->diff()->z());

            diffs.push_back(std::make_pair(node->id(), vec));
        }

        return chrono_types::make_shared<SynSCMMessage::State>(diffs);
    }

    virtual void GenerateState(const SynFlatBuffers::Message* message) override {
        // std::cout << "Bad: Generate State called" << std::endl;
        m_state = SynSCMMessage::GenerateStateFromMessage(message);
    }

    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override {
        auto state = std::static_pointer_cast<SynSCMMessage::State>(m_state);
        std::vector<flatbuffers::Offset<SynFlatBuffers::SCM::Node>> diffs;

        for (auto diff : state->mesh_diffs) {
            chrono::Vector vec = diff.second;
            auto buff_vec = SynFlatBuffers::CreateVector(builder, vec.x(), vec.y(), vec.z());
            diffs.push_back(SynFlatBuffers::SCM::CreateNode(builder, diff.first, buff_vec));
        }

        auto scm_data = SynFlatBuffers::SCM::CreateStateDirect(builder, &diffs);
        FlatBufferMessage message = SynFlatBuffers::CreateMessage(
            builder, SynFlatBuffers::Type_SCM_State, scm_data.Union(), builder.CreateString(std::to_string(m_id)));

        return message;
    }
};
}  // namespace synchrono

#endif  // SYNSCMMESSAGE_H
