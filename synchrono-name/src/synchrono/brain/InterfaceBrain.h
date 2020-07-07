#ifndef INTERFACE_BRAIN_H
#define INTERFACE_BRAIN_H

#include "synchrono/SynApi.h"
#include "synchrono/agent/SynVehicleBrain.h"

#include "synchrono/flatbuffer/SynMessage.h"

#include "synchrono_interface/SynInterface.h"

using namespace synchrono;
using namespace synchrono::interface;

using namespace chrono;

namespace synchrono {
namespace agent {

class SYN_API InterfaceBrain : public SynVehicleBrain {
  public:
    InterfaceBrain(int id,
                   std::shared_ptr<ChDriver> driver,
                   ChVehicle& vehicle,
                   std::shared_ptr<SynInterface> interface);
    ~InterfaceBrain();

    virtual void Synchronize(double time);
    virtual void Advance(double step);
    virtual void ProcessMessage(SynMessage* msg);

  private:
    std::shared_ptr<SynInterface> m_interface;
};

}  // namespace agent
}  // namespace synchrono

#endif  // INTERFACE_BRAIN_H
