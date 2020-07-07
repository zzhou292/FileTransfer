#include "synchrono/flatbuffer/SynMessageFactory.h"

namespace synchrono {

SynMessage* SynMessageFactory::GenerateMessage(const SynFlatBuffers::Message* message) {
    int id = std::stoi(message->id()->str());
    if (message->message_type() == SynFlatBuffers::Type_SCM_State) {
        return new SynSCMMessage(id, SynSCMMessage::GenerateStateFromMessage(message));
    } else if (message->message_type() == SynFlatBuffers::Type_Agent_State) {
        const SynFlatBuffers::Agent::State* agent_state = message->message_as_Agent_State();
        if (agent_state->message_type() == SynFlatBuffers::Agent::Type_Vehicle_State) {
            return new SynVehicleMessage(id, message);
        } else if (agent_state->message_type() == SynFlatBuffers::Agent::Type_TrafficLight_State) {
            return new SynTrafficLightMessage(id, message);
        } else {
            std::cout << "SynMessageFactory::GenerateMessage: Unknown AGENT type. Exitting..." << std::endl;
            exit(-1);
        }
    } else if (message->message_type() == SynFlatBuffers::Type_Sensor_State) {
        return new SynSensorMessage(id, message);
    } else {
        std::cout << "SynMessageFactory::GenerateMessage: Unknown type. Exitting..." << std::endl;
        exit(-1);
    }
    return nullptr;
}

}  // namespace synchrono
