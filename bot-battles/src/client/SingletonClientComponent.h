#ifndef __SINGLETON_CLIENT_COMPONENT_H__
#define __SINGLETON_CLIENT_COMPONENT_H__

namespace sand {

class SocketAddress;
class UDPSocket;

//----------------------------------------------------------------------------------------------------
struct SingletonClientComponent {

    SingletonClientComponent()
        : m_socket(nullptr)
        , m_socketAddress(nullptr)
        , m_name("")
        , m_helloTime(1.0f)
        , m_inputTime(0.033f)
        , m_lastTime(0.0f)
        , m_playerID(INVALID_PLAYER_ID)
    {
    }
    ~SingletonClientComponent() { }

    bool IsConnected() const
    {
        return m_playerID != INVALID_PLAYER_ID;
    }

    F32 GetNextHelloTime() const
    {
        return m_lastTime + m_helloTime;
    }

    F32 GetNextInputTime() const
    {
        return m_lastTime + m_inputTime;
    }

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;
    std::string m_name;
    F32 m_helloTime;
    F32 m_inputTime;
    F32 m_lastTime;
    PlayerID m_playerID;
};
}

#endif
