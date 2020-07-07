#ifndef SYN_MESSAGE_H
#define SYN_MESSAGE_H

#include "synchrono/SynApi.h"
#include "synchrono/flatbuffer/SynFlatBuffers_generated.h"

#include "chrono/core/ChVector.h"
#include "chrono/core/ChQuaternion.h"

using namespace chrono;

namespace synchrono {

typedef flatbuffers::Offset<SynFlatBuffers::Message> FlatBufferMessage;

/// SynMessage is the base class for all messages.
/// Basically wraps the FlatBuffer methods
/// Also contains the state struct, which defines the state of the message/agent
class SYN_API SynMessage {
  public:
    struct Pose {
        ChVector<> pos;
        ChQuaternion<> rot;

        Pose() {}
        Pose(ChVector<> pos, ChQuaternion<> rot) : pos(pos), rot(rot) {}
    };

    struct State {
        double time;

        State(double time) : time(time) {}
        State() : time(0.0) {}
    };

    enum Type { POINTER, MAP, SPAT, VEH, TRAFFIC_LIGHT, SENS, CONTROL, TERRAIN };

    SynMessage(int id, Type type)
        : m_id(id), m_type(type), m_state(chrono_types::make_shared<SynMessage::State>(0.0)) {}
    virtual ~SynMessage() {}

    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) = 0;
    virtual void GenerateState(const SynFlatBuffers::Message* message) = 0;

    Type GetType() { return m_type; }
    unsigned int GetID() { return m_id; }
    double GetTime() { return m_time; }
    std::shared_ptr<State>& GetState() { return m_state; }

  protected:
    int m_id;
    Type m_type;
    double m_time;

    std::shared_ptr<State> m_state;
};

}  // namespace synchrono

#endif
