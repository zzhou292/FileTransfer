#include "synchrono_interface/SynTransportManager.h"

#include <chrono>
#include <iostream>

namespace synchrono {
namespace interface {
SYN_INTERFACE_API SynTransportManager::SynTransportManager(std::string port)
    : m_socket(std::make_shared<boost::asio::ip::tcp::socket>(*new boost::asio::io_service)) {
    // std::cout << "Waiting for connection..." << std::endl;
    boost::asio::ip::tcp::acceptor acceptor(
        m_socket->get_executor(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(port.c_str())));
    acceptor.accept(*m_socket);
    // std::cout << "Connection Established." << std::endl;
}

SynTransportManager::~SynTransportManager() {
    m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    m_socket->close();
}

void SynTransportManager::Send(SynFlatBuffersManager& manager) {
    manager.FinishSizePrefixed();
    int32_t size = manager.GetSize();
    uint8_t* buf = manager.GetBufferPointer();
    size_t send_size = m_socket->send(boost::asio::buffer(buf, size));
    // std::cout << "Send Size:: " << send_size << std::endl;
}

void SynTransportManager::Receive(SynFlatBuffersManager& manager) {
    // Allocate space for message "header"
    std::vector<uint8_t> buffer(4);
    // Receive just the "header"
    boost::asio::read(*m_socket, boost::asio::buffer(buffer.data(), 4));
    // Check the size of the message to read
    int available = ((int*)buffer.data())[0];
    // std::cout << "Available :: " << available << std::endl;
    // Allocate space for the message
    buffer.resize(available);
    // Receive and record size of received packet
    // Read allows us to read tcp buffer until all (int)available are received
    size_t received = boost::asio::read(*m_socket, boost::asio::buffer(buffer.data(), available));
    manager.GetBuffer() = buffer;
}

}  // namespace interface
}  // namespace synchrono
