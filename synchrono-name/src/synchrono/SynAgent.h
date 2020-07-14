#ifndef SYN_AGENT_H
#define SYN_AGENT_H

#include "synchrono/SynApi.h"

#include "synchrono/SynBrain.h"
#include "synchrono/SynVisManager.h"

#include "chrono/physics/ChSystem.h"

using namespace chrono;
using namespace synchrono;
using namespace synchrono::visualization;

namespace synchrono {
namespace agent {

/// Base class for SynChrono agents.
class SYN_API SynAgent {
  public:
    /// Agent types
    enum AgentType {
        VEHICLE,       ///< agent is a vehicle
        TRAFFIC_LIGHT  ///< agent is a traffic light
    };

    /// Construct a agent with the specified rank and type
    SynAgent(unsigned int rank, SynAgent::AgentType type, ChSystem* system = 0)
        : m_rank(rank),
          m_type(type),
          m_system(system),
          m_vis_manager(chrono_types::make_shared<SynVisManager>()),
          m_verbose(false) {}

    /// Destructor.
    ~SynAgent() {}

    /// Advance the state of this agent until agent time syncs with passed time.
    virtual void Advance(double time_of_next_sync) = 0;

    /// Initialize this agents zombie representation.
    virtual void InitializeZombie(ChSystem* system = 0) = 0;

    /// Synchronoize this agents zombie with the rest of the simulation.
    /// Updates agent based on specified message.
    virtual void SynchronizeZombie(SynMessage* message) = 0;

    /// Get agent state
    virtual std::shared_ptr<SynMessage::State> GetState() = 0;

    /// Generate vector of SynMessages to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) = 0;

    // Attach a visualization to this agent.
    void AttachVisManager(std::shared_ptr<SynVisManager> vis_manager) { m_vis_manager = vis_manager; }

    /// Process incoming message. Forwards message to underlying agent brain.
    virtual void ProcessMessage(SynMessage* msg, int sender_rank) { m_brain->ProcessMessage(msg,sender_rank); }

    /// Set this agent's brain
    void SetBrain(std::shared_ptr<SynBrain> brain) { m_brain = brain; }

    /// Set this agent's rank
    void SetRank(unsigned int rank) { m_rank = rank; }

    /// Get the handle to this agent's message that represents its state
    std::shared_ptr<SynMessage> GetMessage() { return m_msg; }

    /// Get the handle to this agent's brain
    std::shared_ptr<SynBrain> GetBrain() { return m_brain; }

    /// Get the type of this agent
    SynAgent::AgentType GetType() const { return m_type; }

    /// Get this agent's rank
    unsigned int GetRank() { return m_rank; }

    /// Get the Chrono system associated with this agent
    ChSystem* GetSystem() { return m_system; }

    /// Set the Chrono system associated with this agent
    void SetSystem(ChSystem* system) { m_system = system; }

    /// Set the verbosity
    void SetVerbosity(bool verbosity) { m_verbose = verbosity; }

  protected:
    unsigned int m_rank;         ///< agent rank
    SynAgent::AgentType m_type;  ///< agent type
    bool m_verbose;              ///< agent verbosity

    std::shared_ptr<SynMessage> m_msg;             ///< handle to this agent's message
    std::shared_ptr<SynBrain> m_brain;             ///< handle to this agent's brain
    std::shared_ptr<SynVisManager> m_vis_manager;  ///< handle to this agent's visualization manager

    ChSystem* m_system;  ///< pointer to the Chrono system
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYN_AGENT_H
