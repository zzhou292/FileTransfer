#ifndef SYN_TRAFFIC_LIGHT_AGENT_H
#define SYN_TRAFFIC_LIGHT_AGENT_H

#include "synchrono/SynApi.h"
#include "synchrono/SynAgent.h"

#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"

#include "chrono/physics/ChBodyEasy.h"
#include "chrono/core/ChVector.h"
#include "chrono/core/ChQuaternion.h"

using namespace chrono;

namespace synchrono {
namespace agent {

class SYN_API SynTrafficLightAgent : public SynAgent {
  public:
    /// Construct SynTrafficLightAgent with the specified rank and ChSystem pointer
    SynTrafficLightAgent(int rank, ChSystem* system = 0);

    /// Destructor
    ~SynTrafficLightAgent();

    /// Advance the state of this vehicle agent until agent time syncs with passed time.
    virtual void Advance(double time_of_next_sync);

    /// Initialize this agents zombie representation.
    /// Will use the passed system if agent system is null.
    virtual void InitializeZombie(ChSystem* system = 0);

    /// Synchronoize this agents zombie with the rest of the simulation.
    /// Updates agent based on specified message.
    virtual void SynchronizeZombie(SynMessage* message);

    /// Generate vector of SynMessages to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages);

    /// Get agent state
    virtual std::shared_ptr<SynMessage::State> GetState();

  private:
    /// Set the color of the passed lanes
    void SetColor(SynSPATMessage::Color color, const std::list<int>& lanes);

    /// Add a lane to the intersection this traffic light is responsible for
    void AddLane(ChVector<> stopPoint, ChVector<> offsetFromStop, double width);

    /// Set current green lights to yellow
    void ChangeToYellow();

    /// Update traffic light state
    void UpdateStates();

    /// Set all lights to read
    void SetAllRed();

  private:
    // agent specific parameters
    double step_size = 0.002;
    double render_step_size = 1.0 / 50;  // FPS = 50

    double currentTime;

    int currState;
    double time_from_last_state;

    const double RGTIME = 10;
    const double YLTIME = 1;
    const double AFTERYLTIME = 1;

    std::shared_ptr<ChBodyEasyBox> m_box;
};
}  // namespace agent
}  // namespace synchrono
#endif
