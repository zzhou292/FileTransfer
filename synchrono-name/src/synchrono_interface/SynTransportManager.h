#ifndef SYN_TRANSPORT_MANAGER_H
#define SYN_TRANSPORT_MANAGER_H

#include <boost/asio.hpp>

#include "synchrono/SynFlatBuffersManager.h"
#include "synchrono_interface/SynApiInterface.h"

namespace synchrono {
namespace interface {

class SYN_INTERFACE_API SynTransportManager {
  public:
    SynTransportManager(std::string port);
    ~SynTransportManager();

    void Send(SynFlatBuffersManager& manager);
    void Receive(SynFlatBuffersManager& manager);

  private:
    std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
};

}  // namespace interface
}  // namespace synchrono

#endif
