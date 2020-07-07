#include "synchrono_interface/SynComponent.h"

#include "synchrono_interface/SynInterfaceDriver.h"

namespace synchrono {
namespace interface {

class SynControlComponent : public SynComponent {
  private:
    SynInterfaceDriver& driver_;

  public:
    SynControlComponent(std::string name, SynInterfaceDriver& driver)
        : SynInterfaceComponent(SynComponentType::Receiver, name), driver_(driver) {}

    std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> toFlatBuffer(
        flatbuffers::FlatBufferBuilder& builder_) {}

    void fromFlatBuffer(const SynInterfaceMessage::Message* message) {
        const SynInterfaceMessage::Control* control =
            static_cast<const SynInterfaceMessage::Control*>(message->message());
        driver_.SetTargetThrottle(control->throttle());
        driver_.SetTargetSteering(control->steering());
        driver_.SetTargetBraking(control->braking());
    }
};
}  // namespace interface
}  // namespace synchrono
