#include "synchrono_interface/SynComponent.h"

#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/filters/ChFilterAccess.h"

using namespace chrono::sensor;

namespace synchrono {
namespace interface {

class SynCameraComponent : public SynComponent {
  private:
    std::shared_ptr<ChCameraSensor> camera_;

  public:
    SynCameraComponent(std::string name, float update_rate, std::shared_ptr<ChCameraSensor> camera)
        : SynInterfaceComponent(SynComponentType::Sender, name, update_rate), camera_(camera) {}

    std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> toFlatBuffer(
        flatbuffers::FlatBufferBuilder& builder_) {
        LockedRGBA8BufferPtr buffer_ptr = camera_->GetMostRecentBuffer<LockedRGBA8BufferPtr>();

        int size = buffer_ptr->Height * buffer_ptr->Width * sizeof(PixelRGBA8);

        flatbuffers::Offset<SynInterfaceMessage::Camera> camera = SynInterfaceMessage::CreateCamera(
            builder_, buffer_ptr->Height, buffer_ptr->Width, sizeof(PixelRGBA8),
            builder_.CreateVector<unsigned char>(reinterpret_cast<uint8_t*>(buffer_ptr->Buffer.get()), size));

        std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> camera_message =
            std::make_shared<flatbuffers::Offset<SynInterfaceMessage::Message>>(SynInterfaceMessage::CreateMessage(
                builder_, SynInterfaceMessage::Type_Camera, camera.Union(), builder_.CreateString(name_)));

        return camera_message;
    }

    void fromFlatBuffer(const SynInterfaceMessage::Message* message) {}
};
}  // namespace interface
}  // namespace synchrono
