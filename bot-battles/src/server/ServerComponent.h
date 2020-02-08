#ifndef __SERVER_COMPONENT_H__
#define __SERVER_COMPONENT_H__

#include "Component.h"

namespace sand {

class UDPSocket;
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

    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& GetPlayerIDToClientProxyMap() const;

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::string m_port;

    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_playerIDToClientProxy;
    std::unordered_map<Entity, PlayerID> m_entityToPlayerID;
    std::queue<PlayerID> m_availablePlayerIDs;

    bool m_isServerRewind;
};
}

#endif
