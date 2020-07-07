#include "synchrono_interface/SynComponent.h"

#include "chrono/physics/ChSystem.h"

namespace synchrono {
namespace interface {

class SynTimeComponent : public SynComponent {
  private:
    ChSystem* system_;

  public:
    SynTimeComponent(std::string name, ChSystem* system)
        : SynInterfaceComponent(SynComponentType::Sender, name), system_(system) {
        MakeConditional();
    }

    std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> toFlatBuffer(
        flatbuffers::FlatBufferBuilder& builder_) {
        flatbuffers::Offset<SynInterfaceMessage::Time> time =
            SynInterfaceMessage::CreateTime(builder_, system_->GetChTime());

        std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> time_message =
            std::make_shared<flatbuffers::Offset<SynInterfaceMessage::Message>>(SynInterfaceMessage::CreateMessage(
                builder_, SynInterfaceMessage::Type_Time, time.Union(), builder_.CreateString(name_)));

        return time_message;
    }

    void fromFlatBuffer(const SynInterfaceMessage::Message* message) {}
};
}  // namespace interface
}  // namespace synchrono
