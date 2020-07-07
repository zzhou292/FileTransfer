#ifndef SYNINTERFACE_H
#define SYNINTERFACE_H

#include "chrono/physics/ChSystem.h"

#include <map>

#include "chrono_vehicle/ChVehicle.h"
#include "synchrono_interface/SynInterfaceDriver.h"

#include "synchrono/flatbuffer/SynFlatBuffersManager.h"

// #include "synchrono_interface/flatbuffers/SynFlatBuffersManager.h"
#include "synchrono_interface/transport/SynTransportManager.h"
#include "synchrono_interface/SynComponent.h"
#include "synchrono_interface/SynApiInterface.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace synchrono;

namespace synchrono {
namespace interface {
class SYN_INTERFACE_API SynInterface {
  public:
    SynInterface(SynFlatBuffersManager& flatbuffers_manager, std::string port);

    void Synchronize(double ch_time);

    void AddComponent(std::shared_ptr<SynComponent> component);

  private:
    std::map<std::string, std::shared_ptr<SynComponent>> m_components;

    SynTransportManager m_transport_manager;
    SynFlatBuffersManager& m_flatbuffers_manager;
};
}  // namespace interface
}  // namespace synchrono

#endif
