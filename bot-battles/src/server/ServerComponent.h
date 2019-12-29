#ifndef __SERVER_COMPONENT_H__
#define __SERVER_COMPONENT_H__

namespace sand {

class UDPSocket;
class SocketAddress;
class ClientProxy;

//----------------------------------------------------------------------------------------------------
// System Component
struct ServerComponent {

    ServerComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    PlayerID AddPlayer(const SocketAddress& socketAddress, const char* name);
    bool RemovePlayer(PlayerID playerID);

    bool AddEntity(Entity entity, PlayerID playerID);
    bool RemoveEntity(Entity entity);

    PlayerID GetPlayerID(const SocketAddress& socketAddress) const;
    Entity GetEntity(const SocketAddress& socketAddress) const;
    std::shared_ptr<ClientProxy> GetClientProxyFromPlayerID(PlayerID playerID) const;
    std::shared_ptr<ClientProxy> GetClientProxyFromEntity(Entity entity) const;
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& GetPlayerIDToClientProxyMap() const;

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::string m_port;

    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_playerIDToClientProxy;
    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_entityToClientProxy;
    std::queue<Entity> m_availablePlayerIDs;
};
}

#endif
