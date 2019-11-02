#ifndef __SINGLETON_SERVER_COMPONENT_H__
#define __SINGLETON_SERVER_COMPONENT_H__

namespace sand {

class UDPSocket;
class ClientProxy;
class SocketAddress;

//----------------------------------------------------------------------------------------------------
struct SingletonServerComponent {

    SingletonServerComponent()
        : m_socket(nullptr)
        , m_socketAddress(nullptr)
        , m_addressToClientProxy()
        , m_playerIDToClientProxy()
    {
    }
    ~SingletonServerComponent() { }

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::unordered_map<SocketAddress, std::shared_ptr<ClientProxy>> m_addressToClientProxy;
    std::unordered_map<U32, std::shared_ptr<ClientProxy>> m_playerIDToClientProxy;
};
}

#endif
