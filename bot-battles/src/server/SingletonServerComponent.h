#ifndef __SINGLETON_SERVER_COMPONENT_H__
#define __SINGLETON_SERVER_COMPONENT_H__

namespace sand {

class UDPSocket;
class SocketAddress;
class ClientProxy;

//----------------------------------------------------------------------------------------------------
struct SingletonServerComponent {

    SingletonServerComponent();
    ~SingletonServerComponent();

    PlayerID AddPlayer(const SocketAddress& socketAddress, const char* name);
    bool RemovePlayer(PlayerID playerID);

    bool AddEntity(Entity entity, PlayerID playerID);
    bool RemoveEntity(Entity entity);

    PlayerID GetPlayerID(const SocketAddress& socketAddress) const;
    std::shared_ptr<ClientProxy> GetClientProxyFromPlayerID(PlayerID playerID) const;
    std::shared_ptr<ClientProxy> GetClientProxyFromEntity(Entity entity) const;
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& GetPlayerIDToClientProxyMap();

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_playerIDToClientProxy;
    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_entityToClientProxy;
    std::queue<Entity> m_availablePlayerIDs;
};
}

#endif
