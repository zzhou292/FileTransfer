#ifndef SYN_VEHICLE_MESSAGE_H
#define SYN_VEHICLE_MESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"

namespace synchrono {

class SYN_API SynVehicleMessage : public SynMessage {
  public:
    /// State struct that holds state information for a SynVehicleAgent
    struct State : SynMessage::State {
        SynMessage::Pose chassis;              ///< vehicle's chassis pose
        std::vector<SynMessage::Pose> wheels;  ///< vector of vehicle's wheels

        /// Default Constructor
        State() : SynMessage::State(0.0) {}

        /// Creates state with specified chassis and wheels
        State(double time, SynMessage::Pose chassis, std::vector<SynMessage::Pose> wheels)
            : SynMessage::State(time), chassis(chassis), wheels(wheels) {}
    };

    /// Constructors
    SynVehicleMessage(int id);
    SynVehicleMessage(int id, std::shared_ptr<SynMessage::State> state);
    SynVehicleMessage(int id, const SynFlatBuffers::Message* message);

    /// Generate state from FlatBuffers message
    virtual void GenerateState(const SynFlatBuffers::Message* message) override;

    /// Generate FlatBuffers message from this message's state
    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override;

  private:
    /// Helper method that generates SynFlatBuffers::Pose from SynMessage::Pose
    flatbuffers::Offset<SynFlatBuffers::Pose> CreatePose(flatbuffers::FlatBufferBuilder& builder,
                                                         SynMessage::Pose& syn_pose);

    /// Helper method that generates SynMessage::Pose from SynFlatBuffers::Pose
    SynMessage::Pose CreatePose(const SynFlatBuffers::Pose* pose);
};
}  // namespace synchrono

#endif
