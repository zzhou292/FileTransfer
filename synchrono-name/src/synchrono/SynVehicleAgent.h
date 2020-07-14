#ifndef SYN_VEHICLE_AGENT_H
#define SYN_VEHICLE_AGENT_H

#include "synchrono/SynApi.h"

#include "synchrono/SynAgent.h"
#include "synchrono/SynVehicleBrain.h"

#include "synchrono/SynTerrain.h"
#include "synchrono/flatbuffer/message/SynSCMMessage.h"

#include "synchrono/flatbuffer/message/SynVehicleMessage.h"

#include "chrono_vehicle/ChVehicle.h"
#include "chrono_vehicle/wheeled_vehicle/ChWheeledVehicle.h"
#include "chrono_vehicle/ChDriver.h"
#include "chrono_vehicle/ChVehicleModelData.h"

using namespace chrono;
using namespace chrono::vehicle;

namespace synchrono {
namespace agent {

class SYN_API SynVehicleAgent : public SynAgent {
  public:
    /// Vehicle agent type
    enum VehicleType {
        SEDAN,    ///> vehicle agent is a sedan
        CITYBUS,  ///> vehicle agent is a citybus
        HMMWV,     ///> vehicle agent is a hmmwv
        HMMWV_FULL
    };

    /// Construct a vehicle agent with the specified rank and type
    /// Underlying agent is set to a vehicle type automatically
    SynVehicleAgent(unsigned int rank, VehicleType type, ChSystem* system = 0);

    /// Destructor
    ~SynVehicleAgent() {}

    /// Initialize vehicle with the specified location and orientation, using the passed Chrono system.
    virtual void InitializeVehicle(const ChCoordsys<>& pose) = 0;

    /// Advance the state of this vehicle agent until agent time syncs with passed time.
    virtual void Advance(double time_of_next_sync);

    /// Initialize this agents zombie representation.
    /// Will use the passed system if agent system is null.
    virtual void InitializeZombie(ChSystem* system = 0);

    /// Process incoming message. Forwards message to underlying agent brain.
    virtual void ProcessMessage(SynMessage* msg, int sender_rank);

    /// Synchronoize this agents zombie with the rest of the simulation.
    /// Updates agent based on specified message.
    void SynchronizeZombie(SynMessage* message);

    /// Get agent state
    virtual std::shared_ptr<SynMessage::State> GetState();

    /// Generate vector of SynMessages to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messsages);

    /// Get this agent's vehicle
    virtual ChVehicle& GetVehicle() = 0;

    /// Get this agent's as a wheeled vehicle
    virtual ChWheeledVehicle& GetWheeledVehicle() { return static_cast<ChWheeledVehicle&>(GetVehicle()); }

    /// Get Chrono driver from brain
    std::shared_ptr<ChDriver> GetDriver() { return std::static_pointer_cast<SynVehicleBrain>(m_brain)->GetDriver(); }

    /// Set the terrain for this vehicle
    void SetTerrain(std::shared_ptr<SynTerrain> terrain) { m_terrain = terrain; }

    /// Get handle to this agents terrain
    std::shared_ptr<SynTerrain> GetTerrain() { return m_terrain; }

    /// Get this vehicle agent's type
    SynVehicleAgent::VehicleType GetVehicleType() { return m_vehicle_type; }

    /// Set the step size of the simulation
    void SetStepSize(double step_size) { m_step_size = step_size; }

    /// Get zombie body
    std::shared_ptr<ChBodyAuxRef> GetZombieBody() { return m_zombie_body; }

    virtual void UpdateMyLoc(chrono::Vector);

  protected:
    double m_step_size = 1e-3;  ///< default simulation step size

    std::shared_ptr<SynTerrain> m_terrain;  ///< handle to this agent's terrain

    bool m_owns_system;  ///< true if system created at construction

    SynVehicleAgent::VehicleType m_vehicle_type;  ///< vehicle agent type

    std::string m_chassis_vis_file = "";  ///< file name for chassis visualization
    std::string m_wheel_vis_file = "";    ///< file name for wheel visualization
    std::string m_tire_vis_file = "";     ///< file name for tire visualization

    std::shared_ptr<ChBodyAuxRef> m_zombie_body;              ///< agent's zombie body reference
    std::vector<std::shared_ptr<ChBodyAuxRef>> m_wheel_list;  ///< vector of this agent's zombie wheels
};
}  // namespace agent
}  // namespace synchrono
#endif
