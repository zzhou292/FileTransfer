#ifndef SYN_V2V_BRAIN_H
#define SYN_V2V_BRAIN_H

#include "InferenceDriver.h"
#include "chrono_vehicle/driver/ChPathFollowerACCDriver.h"

#include "synchrono/SynApi.h"
#include "synchrono/SynVehicleBrain.h"

#include "synchrono/flatbuffer/message/SynSensorMessage.h"

#include "chrono/geometry/ChBox.h"

#include "chrono_sensor/ChGPSSensor.h"
#include "chrono_sensor/ChLidarSensor.h"

using namespace synchrono;

using namespace chrono;
using namespace chrono::geometry;

namespace synchrono {
namespace agent {

class SYN_API SynV2VBrain : public SynVehicleBrain {
  public:
    /// Constructor
    SynV2VBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle, std::shared_ptr<ChGPSSensor> gps);

    /// Destructor
    ~SynV2VBrain() {}

    /// Advance the state of this brain until brain time syncs with passed time.
    virtual void Advance(double step);

    /// Process an incoming message
    virtual void ProcessMessage(SynMessage* msg) override;

    /// Generate vector of SynMessage's to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) override;

  private:
    /// Check if the leader is close
    bool LeaderIsClose();

  private:
    int m_receive_rank;  ///< rank which inference brain should listen to incoming messages

    // Used for calculating whether a vehicle is infront of the car
    ChBox m_box;  ///< bounding box where vehicle should check if another vehicle is present

    std::shared_ptr<ChGPSSensor> m_gps;      ///< Handle to the GPS sensor
    std::shared_ptr<ChLidarSensor> m_lidar;  ///< Handle to the lidar sensor

    ChVector<> m_leader_pos;  ///< Position of the leader vehicle
    ChVector<> m_origin;      ///< Origin where coordinate conversion from GPS to cartesian should be made
};

}  // namespace agent
}  // namespace synchrono

#endif  // PARK_ST_BRAIN_H
