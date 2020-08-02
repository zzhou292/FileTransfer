#ifndef SYN_MPI_MANAGER_H
#define SYN_MPI_MANAGER_H

#include <mpi.h>
#include <map>

#include "synchrono/SynApi.h"

#include "synchrono/SynAgent.h"
#include "synchrono/SynVehicleAgent.h"

#include "synchrono/SynFlatBuffersManager.h"
#include "synchrono/flatbuffer/SynMessage.h"
#include "synchrono/flatbuffer/SynMessageFactory.h"
#include "synchrono/flatbuffer/message/SynPointerMessage.h"

using namespace synchrono;
using namespace synchrono::agent;

namespace synchrono {
namespace mpi {

/// Manager of MPI operations in one sychrono simulation
class SYN_API SynMPIManager {
  public:
    /// Consruct a mpi manager object
    SynMPIManager(int argc, char* argv[]);

    /// Destructor
    ~SynMPIManager();

    /// Set the agent at the specified rank
    void AddAgent(std::shared_ptr<SynAgent> agent, int rank, bool verbose = false);

    /// Initialize agents and their zombies
    void Initialize();

    /// Gather updated state info and msgs to send of agents on this rank
    void Advance(double time);

    /// Gather state info of and msgs from agents in other ranks
    void Synchronize();

    /// Broadcast msgs and state infos to agents on other ranks
    void Broadcast();

    /// Update the zombie agents
    void Update();

    /// Wrapper of MPI barrier
    void Barrier() { MPI_Barrier(MPI_COMM_WORLD); }

    /// Get agent at specified rank
    std::shared_ptr<SynAgent> GetAgent(int rank) { return m_agent_list[rank]; }

    /// Get rank of current mpi manager
    int GetRank() { return m_rank; }

    /// Get number of ranks
    int GetNumRanks() { return m_num_ranks; }

    /// Get the agent list
    std::map<int, std::shared_ptr<SynAgent>> GetAgentList() { return m_agent_list; }

    void Track(chrono::Vector);

    //std::shared_ptr<chrono::Vector> ReturnTrackingData(int rank);

    //void PrintLocData();

    //void PrintDistance();

  private:
    int m_rank;       ///< rank which this manager controls
    int m_num_ranks;  ///< total number of ranks in this simulation

    //This vectors stores the location of the agent on the current rank
    chrono::Vector myLoc;

    std::map<int, std::shared_ptr<SynAgent>> m_agent_list;  ///< id to agent map on this rank

    std::shared_ptr<SynMessage::State> m_agent_state;  ///< handle to agents state info on this rank

    SynFlatBuffersManager m_flatbuffers_manager;  ///< flatbuffer manager for this rank

    
};
}  // namespace mpi
}  // namespace synchrono
#endif  // SYN_MPI_MANAGER_H
