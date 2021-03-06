#ifndef __SERVER_COMPONENT_H__
#define __SERVER_COMPONENT_H__

#include "Component.h"

namespace sand {

class UDPSocket;
class TCPSocket;
class SocketAddress;
class ClientProxy;

//----------------------------------------------------------------------------------------------------
// System Component
struct ServerComponent : public Component {

    ServerComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    PlayerID AddPlayer(const SocketAddress& socketAddress, const char* name);
    bool RemovePlayer(PlayerID playerID);

    bool AddEntity(Entity entity, PlayerID playerID);
    bool RemoveEntity(Entity entity);

    PlayerID GetPlayerID(const SocketAddress& socketAddress) const;
    std::weak_ptr<ClientProxy> GetClientProxy(PlayerID playerID) const;
    PlayerID GetPlayerID(Entity entity) const;
    Entity GetEntity(PlayerID playerID) const;
    U32 GetPlayerCount() const;
    U32 GetEntityCount() const;
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& GetPlayerIDToClientProxyMap() const;

    bool RemoveTCPSocket(const SocketAddress& socketAddress);

    std::weak_ptr<TCPSocket> GetTCPSocket(const SocketAddress& socketAddress) const;

    std::shared_ptr<UDPSocket> m_UDPSocket;
    std::weak_ptr<TCPSocket> m_TCPListenSocket;
    std::vector<std::shared_ptr<TCPSocket>> m_TCPSockets;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::string m_port;

    std::string m_map;

    I32 m_duration;
    MyTimer m_durationTimer;

    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_playerIDToClientProxy;
    std::unordered_map<Entity, PlayerID> m_entityToPlayerID;
    std::priority_queue<PlayerID, std::vector<PlayerID>, std::greater<PlayerID>> m_availablePlayerIDs;

    MyTimer m_incomingPacketsTimer;
    MyTimer m_outgoingPacketsTimer;

    F32 m_incomingPacketsTimeout;
    F32 m_outgoingPacketsTimeout;
    F32 m_disconnectTimeout;
    F32 m_ackTimeout;

    U32 m_maxPacketsPerFrame;
    U32 m_maxInputsPerPaquet;

    bool m_isServerRewind;
};
}

#endif
