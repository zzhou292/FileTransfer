#pragma once

#include <string>

// #include "synchrono_interface/flatbuffers/SynInterfaceMessage_generated.h"

#include "synchrono_flatbuffers/SynFlatBuffers_generated.h"
#include "synchrono_flatbuffers/messages/SynMessage.h"

#include "synchrono_interface/SynApiInterface.h"

namespace synchrono {
namespace interface {

class SYN_INTERFACE_API SynComponent {
  public:
    enum Type { SENDER, RECEIVER, SENDERANDRECEIVER, UPDATER };

    SynComponent(Type type, int id, float update_rate) : m_type(type), m_id(id), m_update_rate(update_rate) {}

    SynComponent(Type type, int id) : m_type(type), m_id(id) {}

    SynComponent(Type type) : m_type(type) {}

    virtual void Advance(double time) = 0;

    Type GetType() { return m_type; }

    int GetID() { return m_id; }
    void SetID(int id) { m_id = id; }

    float GetUpdateRate() { return m_update_rate; }
    void SetUpdateRate(float update_rate) { m_update_rate = update_rate; }

    unsigned int GetNumUpdates() { return m_num_updates; }
    void IncrementNumUpdates() { m_num_updates++; }

    bool ShouldUpdate(double ch_time) { return ch_time * m_update_rate + 1e-5 > m_num_updates; }

    bool IsConditional() { return m_is_conditional; }
    void MakeConditional() { m_is_conditional = true; }

    std::shared_ptr<SynMessage> GetMessage() { return m_msg; }

  protected:
    int m_id;

    float m_update_rate = 0;

    unsigned int m_num_updates = 0;

    Type m_type;

    bool m_is_conditional = false;

    std::shared_ptr<SynMessage> m_msg;
};
}  // namespace interface
}  // namespace synchrono
