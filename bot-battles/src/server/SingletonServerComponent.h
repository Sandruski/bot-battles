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

    PlayerID GetPlayerID(const SocketAddress& socketAddress) const;
    std::shared_ptr<ClientProxy> GetClientProxy(PlayerID playerID) const;
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& GetPlayerIDToClientProxyMap();

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> m_playerIDToClientProxy;
    std::queue<Entity> m_availablePlayerIDs;
};
}

#endif
