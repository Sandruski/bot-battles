#include "SingletonServerComponent.h"

#include "ClientProxy.h"
#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SingletonServerComponent::SingletonServerComponent()
    : m_socket(nullptr)
    , m_socketAddress(nullptr)
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
    PlayerID playerID = GetPlayerID(socketAddress);
    if (playerID != INVALID_PLAYER_ID) {
        WLOG("Player %s with the address %s is already registered", name, socketAddress.GetName());
        return playerID;
    }

    playerID = m_availablePlayerIDs.front();
    m_availablePlayerIDs.pop();
    std::shared_ptr<ClientProxy> clientProxy = std::make_shared<ClientProxy>(socketAddress, name);
    m_playerIDToClientProxy[playerID] = clientProxy;

    return playerID;
}

//----------------------------------------------------------------------------------------------------
bool SingletonServerComponent::RemovePlayer(const SocketAddress& socketAddress)
{
    PlayerID playerID = GetPlayerID(socketAddress);
    if (playerID == INVALID_PLAYER_ID) {
        WLOG("Player with the address %s is not registered", socketAddress.GetName());
        return false;
    }

    m_playerIDToClientProxy.erase(playerID);
    m_availablePlayerIDs.push(playerID);

    return true;
}

//----------------------------------------------------------------------------------------------------
PlayerID SingletonServerComponent::GetPlayerID(const SocketAddress& socketAddress) const
{
    for (const auto& pair : m_playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        if (socketAddress == clientProxy->GetSocketAddress()) {
            return pair.first;
        }
    }

    return INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<ClientProxy> SingletonServerComponent::GetClientProxy(const SocketAddress& socketAddress) const
{
    for (const auto& pair : m_playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        if (socketAddress == clientProxy->GetSocketAddress()) {
            return pair.second;
        }
    }

    return nullptr;
}
}
