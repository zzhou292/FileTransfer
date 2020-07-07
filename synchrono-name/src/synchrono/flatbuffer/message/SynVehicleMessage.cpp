#include "synchrono/flatbuffer/message/SynVehicleMessage.h"

namespace synchrono {

/// Constructors
SynVehicleMessage::SynVehicleMessage(int id) : SynMessage(id, SynMessage::VEH) {}
SynVehicleMessage::SynVehicleMessage(int id, std::shared_ptr<SynMessage::State> state)
    : SynMessage(id, SynMessage::VEH) {
    m_state = std::static_pointer_cast<State>(state);
}
SynVehicleMessage::SynVehicleMessage(int id, const SynFlatBuffers::Message* message) : SynMessage(id, SynMessage::VEH) {
    GenerateState(message);
}

/// Generate state from FlatBuffers message
void SynVehicleMessage::GenerateState(const SynFlatBuffers::Message* message) {
    /// System of casts from SynFlatBuffers::Message to SynFlatBuffers::Agent::Vehicle::State
    const SynFlatBuffers::Agent::State* agent_state = message->message_as_Agent_State();
    const SynFlatBuffers::Agent::Vehicle::State* state = agent_state->message_as_Vehicle_State();

    std::vector<SynMessage::Pose> wheels;
    for (auto wheel : (*state->wheels()))
        wheels.push_back(CreatePose(wheel));

    SynMessage::Pose chassis = CreatePose(state->chassis());

    m_state = chrono_types::make_shared<State>(state->time(), chassis, wheels);
}

/// Generate FlatBuffers message from this message's state
FlatBufferMessage SynVehicleMessage::GenerateMessage(flatbuffers::FlatBufferBuilder& builder) {
    std::shared_ptr<State> state = std::static_pointer_cast<State>(m_state);

    auto chassis = CreatePose(builder, state->chassis);
    std::vector<flatbuffers::Offset<SynFlatBuffers::Pose>> wheels;
    for (auto wheel : state->wheels) {
        wheels.push_back(CreatePose(builder, wheel));
    }

    auto flatbuffer_state = SynFlatBuffers::Agent::CreateState(
        builder, SynFlatBuffers::Agent::Type_Vehicle_State,
        SynFlatBuffers::Agent::Vehicle::CreateState(builder, state->time, chassis, builder.CreateVector(wheels))
            .Union());

    FlatBufferMessage message = flatbuffers::Offset<SynFlatBuffers::Message>(
        SynFlatBuffers::CreateMessage(builder, SynFlatBuffers::Type_Agent_State, flatbuffer_state.Union(),
                                      builder.CreateString(std::to_string(m_id))));
    return message;
}

/// Helper method that generates SynFlatBuffers::Pose from SynMessage::Pose
flatbuffers::Offset<SynFlatBuffers::Pose> SynVehicleMessage::CreatePose(flatbuffers::FlatBufferBuilder& builder,
                                                                        SynMessage::Pose& syn_pose) {
    auto pos = SynFlatBuffers::CreateVector(builder, syn_pose.pos.x(), syn_pose.pos.y(), syn_pose.pos.z());
    auto rot = SynFlatBuffers::CreateQuaternion(builder, syn_pose.rot.e0(), syn_pose.rot.e1(), syn_pose.rot.e2(),
                                                syn_pose.rot.e3());
    auto fb_pose = SynFlatBuffers::CreatePose(builder, pos, rot);
    return fb_pose;
}

/// Helper method that generates SynMessage::Pose from SynFlatBuffers::Pose
SynMessage::Pose SynVehicleMessage::CreatePose(const SynFlatBuffers::Pose* pose) {
    auto pos = ChVector<>(pose->pos()->x(), pose->pos()->y(), pose->pos()->z());
    auto rot = ChQuaternion<>(pose->rot()->e0(), pose->rot()->e1(), pose->rot()->e2(), pose->rot()->e3());

    return {pos, rot};
}

}  // namespace synchrono
