#ifndef __SINGLETON_CLIENT_COMPONENT_H__
#define __SINGLETON_CLIENT_COMPONENT_H__

#include "ComponentDefs.h"

namespace sand {

class SocketAddress;
class UDPSocket;

//----------------------------------------------------------------------------------------------------
struct SingletonClientComponent {

    enum class ClientState {
        UNINITIALIZED,
        SAY_HELLO,
        SEND_INPUT,

        COUNT,
        INVALID
    };

    SingletonClientComponent()
        : m_socket(nullptr)
        , m_socketAddress(nullptr)
        , m_name("")
        , m_state(ClientState::UNINITIALIZED)
        , m_timeBetweenSayHello(1.0f)
        , m_timeBetweenSendInput(0.033f)
        , m_lastTime(0.0f)
        , m_playerID(0)
    {
    }
    ~SingletonClientComponent() { }

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;
    std::string m_name;
    ClientState m_state;
    F32 m_timeBetweenSayHello;
    F32 m_timeBetweenSendInput;
    F32 m_lastTime;
    U32 m_playerID;
};
}

#endif
