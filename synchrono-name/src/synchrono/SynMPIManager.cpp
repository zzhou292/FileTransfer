#include "synchrono/SynMPIManager.h"

namespace synchrono {
namespace mpi {

// Constructor
SynMPIManager::SynMPIManager(int argc, char* argv[]) {
    // mpi initialization
    MPI_Init(&argc, &argv);
    // set rank
    MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
    // set number of ranks in this simulation
    MPI_Comm_size(MPI_COMM_WORLD, &m_num_ranks);
}

// Destructor
SynMPIManager::~SynMPIManager() {
    MPI_Finalize();
}

// Set the agent at the specified rank
void SynMPIManager::AddAgent(std::shared_ptr<SynAgent> agent, int rank, bool verbose) {
    if (rank == m_rank) {
        auto it = m_agent_list.find(m_rank);
        if (it != m_agent_list.end()) {
            if (verbose)
                std::cout << "SynMPIManager::AddAgent: Each agent should have a unique rank." << std::endl;
            return;
        }
    }

    // Ignore zeroth rank
    if (rank == 0) {
        if (verbose)
            std::cout << "SynMPIManager::AddAgent: Ignoring zeroth rank." << std::endl;
        return;
    }

    m_agent_list[rank] = agent;
}

// Initialize agents and their zombies
void SynMPIManager::Initialize() {
    if (!m_rank)
        return;

    std::shared_ptr<SynAgent> agent = m_agent_list[m_rank];
    for (int i = 1; i < m_num_ranks; i++) {
        if (i != m_rank && agent->GetSystem())
            m_agent_list[i]->InitializeZombie(agent->GetSystem());
    }
}

// Gather updated state info and msgs to send of agents on this rank
void SynMPIManager::Advance(double time) {
    // currently one agent per rank
    if (m_rank != 0)
        m_agent_list[m_rank]->Advance(time);

    // Reset FlatBuffer buffer
    m_flatbuffers_manager.Reset();
}

// Gather state info of and msgs from agents in other ranks
void SynMPIManager::Synchronize() {
    // synchronize between ranks
    if (m_rank == 0) {
        double max_sync_error = 0;
        int size = 0;
        for (int i = 1; i < m_num_ranks; i++) {
            MPI_Status status;
            int nbytes;

            MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
            // It will block the caller until a message is ready

            // Obtain message size...
            MPI_Get_count(&status, MPI_BYTE, &nbytes);

            // Allocate memory to receive data
            uint8_t* buff;
            if (nbytes != MPI_UNDEFINED) {
                buff = (uint8_t*)malloc(nbytes);
            }

            // Finally, receive the message with a correctly sized buffer...
            MPI_Recv(buff, nbytes, MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            SynPointerMessage* msg = new SynPointerMessage(m_rank, buff, nbytes);
            m_flatbuffers_manager.AddMessage(msg);
            // Clean up
            free(buff);
            delete msg;
        }
        m_flatbuffers_manager.Finish();

    } else {
        std::vector<SynMessage*> messages;
        m_agent_list[m_rank]->GenerateMessagesToSend(messages);
        for (auto msg : messages) {
            m_flatbuffers_manager.AddMessage(msg);
            delete msg;
        }

        m_flatbuffers_manager.Finish();

        MPI_Send(m_flatbuffers_manager.GetBufferPointer(), m_flatbuffers_manager.GetSize(), MPI_BYTE, 0, 0,
                 MPI_COMM_WORLD);
    }
}

// Broadcast msgs and state infos to agents on other ranks
void SynMPIManager::Broadcast() {
    // send the entire set of collected states to all the ranks
    int size = m_flatbuffers_manager.GetSize();

    if (m_rank == 0) {
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(m_flatbuffers_manager.GetBufferPointer(), size, MPI_BYTE, 0, MPI_COMM_WORLD);
    } else {
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        m_flatbuffers_manager.GetBuffer().resize(size);
        MPI_Bcast(m_flatbuffers_manager.GetBuffer().data(), size, MPI_BYTE, 0, MPI_COMM_WORLD);
    }
//-------------------------------------------------------------
//--------------------------------------------------------------
//------------------------------------------------------------
    //if(m_rank == 1){
    //     MPI_Bcast(&location_1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //}else if(m_rank == 2){
    //     MPI_Bcast(&location_2, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //}
}

// Update the zombie agents
void SynMPIManager::Update() {
    // jht: Really we only want to update zombies for ranks which hold a Chrono system
    // which atm is just Vehicles. Do it this way because only SynMPIManager has agent list
    // jht: Add SimulationManager agent, then have this switch all over SynMPIManager
    if (!m_rank)
        return;

    for (int i = 1; i < m_num_ranks; i++) {
        if (i != m_rank) {
            /// System of casts from SynFlatBuffers::Pointer to SynFlatBuffers::Message
            auto pointer = static_cast<const SynFlatBuffers::Pointer*>(m_flatbuffers_manager.Get(i)->message());
            auto buffer = flatbuffers::GetRoot<SynFlatBuffers::Buffer>(pointer->pointer()->Data());
            for (auto message : (*buffer->buffer())) {
                SynMessage* msg = SynMessageFactory::GenerateMessage(message);
                // advance the agent to a specific time where we will synchronize
                // Only synchronize zombie if this rank's agent has a ChSystem
                // Must do this because otherwise, the underlying zombie agents won't have a system either
                // Would still like to process message at some point
                if (m_agent_list[m_rank]->GetSystem())
                    m_agent_list[i]->SynchronizeZombie(msg);
                m_agent_list[m_rank]->ProcessMessage(msg, i);
                delete msg;
            }
        }
    }
}

void SynMPIManager::Track(chrono::Vector Sen){
    myLoc = Sen;
    //std::cout<<myLoc<<std::endl;
    m_agent_list[m_rank]->GetBrain()->UpdateMyLoc(Sen);
}



}  // namespace mpi
}  // namespace synchrono
