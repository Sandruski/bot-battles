#include "SingletonServerComponent.h"

#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SingletonServerComponent::SingletonServerComponent()
    : m_socket(nullptr)
    , m_socketAddress(nullptr)
    , m_socketAddressToPlayerID()
    , m_playerIDToClientProxy()
    , m_availablePlayerIDs()
{
    for (PlayerID i = 0; i < MAX_PLAYER_IDS; ++i) {
        m_availablePlayerIDs.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
SingletonServerComponent::~SingletonServerComponent()
{
}

//----------------------------------------------------------------------------------------------------
PlayerID SingletonServerComponent::AddPlayer(const SocketAddress& socketAddress, const char* name)
{
    auto it = m_socketAddressToPlayerID.find(socketAddress);
    if (it != m_socketAddressToPlayerID.end()) {
        WLOG("Player %s with the address %s is already registered and cannot be registered again", name, socketAddress.GetAsString());
        return *it;
    }

    PlayerID playerID = m_availablePlayerIDs.front();
    m_availablePlayerIDs.pop();
    m_socketAddressToPlayerID[socketAddress] = playerID;
    std::shared_ptr<ClientProxy> clientProxy = std::make_shared<ClientProxy>(name);
    m_playerIDToClientProxy[playerID] = clientProxy;

    return playerID;
}

//----------------------------------------------------------------------------------------------------
bool SingletonServerComponent::RemovePlayer(const SocketAddress& socketAddress)
{
    auto it = m_socketAddressToPlayerID.find(socketAddress);
    if (it == m_socketAddressToPlayerID.end()) {
        WLOG("Player with the address %s is not registered and cannot be removed", socketAddress.GetAsString());
        return false;
    }

    PlayerID playerID = *it;
    m_socketAddressToPlayerID.erase(socketAddress);
    m_playerIDToClientProxy.erase(playerID);
    m_availablePlayerIDs.push(playerID);

    return true;
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<ClientProxy> SingletonServerComponent::GetClientProxy(const SocketAddress& socketAddress) const
{
    auto playerIDIt = m_socketAddressToPlayerID.find(socketAddress);
    if (playerIDIt == m_socketAddressToPlayerID.end()) {
        WLOG("The socket address %s has no associated playerID", socketAddress.GetAsString());
        return nullptr;
    }

    PlayerID playerID = *playerIDIt;
    auto clientProxyIt = m_playerIDToClientProxy.find(playerID);
    if (clientProxyIt == m_socketAddressToPlayerID.end()) {
        WLOG("The playerID %u has no associated client proxy", playerID);
        return nullptr;
    }

    return *clientProxyIt;
}
}
