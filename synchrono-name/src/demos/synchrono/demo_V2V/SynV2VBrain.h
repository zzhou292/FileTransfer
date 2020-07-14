#ifndef SYN_V2V_BRAIN_H
#define SYN_V2V_BRAIN_H

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"

#include "synchrono/flatbuffer/message/SynSensorMessage.h"

#include "chrono_sensor/ChGPSSensor.h"
#include "chrono_sensor/ChIMUSensor.h"

using namespace synchrono;

using namespace chrono::sensor;

namespace synchrono {
namespace agent {

class SYN_API SynV2VBrain : public SynVehicleBrain {
  public:
    /// Constructor
    SynV2VBrain(int rank,
                std::shared_ptr<ChDriver> driver,
                ChVehicle& vehicle,
                std::shared_ptr<ChGPSSensor> gps = nullptr,
                std::shared_ptr<ChIMUSensor> imu = nullptr);

    /// Destructor
    ~SynV2VBrain() {}

    /// Process an incoming message
    virtual void ProcessMessage(SynMessage* msg,int sender_rank) override;

    /// Generate vector of SynMessage's to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) override;

  private:
    std::shared_ptr<ChGPSSensor> m_gps;  ///< Handle to the GPS sensor
    std::shared_ptr<ChIMUSensor> m_imu;  ///< Handle to the IMU sensor
};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
