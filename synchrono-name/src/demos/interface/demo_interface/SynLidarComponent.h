#pragma once

#include "synchrono_interface/SynComponent.h"

#include "chrono_sensor/ChLidarSensor.h"
#include "chrono_sensor/filters/ChFilterAccess.h"

using namespace chrono::sensor;

namespace synchrono {
namespace interface {

class SynLidarComponent : public SynComponent {
  private:
    std::shared_ptr<ChLidarSensor> lidar_;

  public:
    SynLidarComponent(std::string name, float update_rate, std::shared_ptr<ChLidarSensor> lidar)
        : SynInterfaceComponent(SynComponentType::Sender, name, update_rate), lidar_(lidar) {}

    std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> toFlatBuffer(
        flatbuffers::FlatBufferBuilder& builder_) {
        LockedXYZIBufferPtr buffer_ptr = lidar_->GetMostRecentBuffer<LockedXYZIBufferPtr>();

        int size = buffer_ptr->Height * buffer_ptr->Width * sizeof(PixelXYZI);

        flatbuffers::Offset<SynInterfaceMessage::Lidar> lidar = SynInterfaceMessage::CreateLidar(
            builder_, sizeof(PixelXYZI),
            builder_.CreateVector<unsigned char>(reinterpret_cast<uint8_t*>(buffer_ptr->Buffer.get()), size));

        std::shared_ptr<flatbuffers::Offset<SynInterfaceMessage::Message>> lidar_message =
            std::make_shared<flatbuffers::Offset<SynInterfaceMessage::Message>>(SynInterfaceMessage::CreateMessage(
                builder_, SynInterfaceMessage::Type_Lidar, lidar.Union(), builder_.CreateString(name_)));

        return lidar_message;
    }

    void fromFlatBuffer(const SynInterfaceMessage::Message* message) {}
};
}  // namespace interface
}  // namespace synchrono
