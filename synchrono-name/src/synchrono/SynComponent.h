#ifndef SYN_COMP_H
#define SYN_COMP_H

#include "synchrono/SynApi.h"

#include "synchrono/flatbuffer/SynMessage.h"

namespace synchrono {
namespace agent {

class SYN_API SynComponent {
  public:
    /// Constructor
    SynComponent(int rank) : m_rank(rank) {}

    /// Destructor
    ~SynComponent() {}

    /// Advance the state of this brain until brain time syncs with passed time
    virtual void Advance(double step) = 0;

    /// Synchronize this brain to the specified time
    virtual void Synchronize(double time) = 0;

    /// Process an incoming message
    virtual void ProcessMessage(SynMessage* msg,int sender_rank) = 0;

    /// Generate vector of SynMessage's to send
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) {}

    /// Get this brains rank
    int GetRank() { return m_rank; }

  protected:
    int m_rank;  ///< rank of this brain
};

}  // namespace agent
}  // namespace synchrono

#endif  // SYN_BRAIN_H
