#ifndef __SINGLETON_CLIENT_COMPONENT_H__
#define __SINGLETON_CLIENT_COMPONENT_H__

#include "Move.h"

namespace sand {

class SocketAddress;
class UDPSocket;
struct SingletonInputComponent;

//----------------------------------------------------------------------------------------------------
struct SingletonClientComponent {

    SingletonClientComponent()
        : m_moves()
        , m_socket(nullptr)
        , m_socketAddress(nullptr)
        , m_name("ExamplePlayer")
        , m_helloTime(1.0f)
        , m_inputTime(0.03f)
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

    const Move& AddMove(const SingletonInputComponent& input, F32 timestamp);
    const Move& GetMove(U32 index) const;

    U32 GetMoveCount() const
    {
        return m_moves.size();
    }

    bool HasMoves() const
    {
        return !m_moves.empty();
    }

    void ClearMoves()
    {
        m_moves.clear();
    }

    std::deque<Move> m_moves;

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
