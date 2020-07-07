#ifndef SYN_MESSAGE_FACTORY_H
#define SYN_MESSAGE_FACTORY_H

#include "synchrono/SynApi.h"
#include "synchrono/flatbuffer/SynMessage.h"
#include "synchrono/flatbuffer/message/SynSensorMessage.h"
#include "synchrono/flatbuffer/message/SynMAPMessage.h"
#include "synchrono/flatbuffer/message/SynPointerMessage.h"
#include "synchrono/flatbuffer/message/SynSCMMessage.h"
#include "synchrono/flatbuffer/message/SynSPATMessage.h"
#include "synchrono/flatbuffer/message/SynTrafficLightMessage.h"
#include "synchrono/flatbuffer/message/SynVehicleMessage.h"

namespace synchrono {

/// Generates SynMessage in various ways
/// Used to improve generality in Agent classes
class SYN_API SynMessageFactory {
  public:
    /// Generate the corresponding SynMessage from a SynFlatBuffers::Message*
    static SynMessage* GenerateMessage(const SynFlatBuffers::Message* message);
};

}  // namespace synchrono

#endif
