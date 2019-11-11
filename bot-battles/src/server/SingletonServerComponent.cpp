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
    m_playerIDToClientProxy.reserve(MAX_PLAYER_IDS);

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
        std::shared_ptr<ClientProxy> clientProxy = GetClientProxy(playerID);
        if (COMPARE_STRINGS(clientProxy->GetName(), name)) {
            WLOG("Player with the socket address %s is already registered", socketAddress.GetName());
        } else {
            WLOG("Player with the socket address %s is already registered with a different name", socketAddress.GetName());
        }
        return playerID;
    }

    playerID = m_availablePlayerIDs.front();
    m_availablePlayerIDs.pop();
    std::shared_ptr<ClientProxy> clientProxy = std::make_shared<ClientProxy>(socketAddress, name);
    std::pair<std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>::iterator, bool> pair = m_playerIDToClientProxy.insert(std::make_pair(playerID, clientProxy));
    if (!pair.second) {
        WLOG("Player with the socket address %s could not be registered", socketAddress.GetName());
        m_availablePlayerIDs.push(playerID);
        return INVALID_PLAYER_ID;
    }

    return playerID;
}

//----------------------------------------------------------------------------------------------------
bool SingletonServerComponent::RemovePlayer(PlayerID playerID)
{
    auto it = m_playerIDToClientProxy.find(playerID);
    if (it == m_playerIDToClientProxy.end()) {
        WLOG("Player %u is not registered", playerID);
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
        if (clientProxy->GetSocketAddress() == socketAddress) {
            return pair.first;
        }
    }

    return INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<ClientProxy> SingletonServerComponent::GetClientProxy(PlayerID playerID) const
{
    auto it = m_playerIDToClientProxy.find(playerID);
    if (it == m_playerIDToClientProxy.end()) {
        WLOG("Player %u is not registered", playerID);
        return nullptr;
    }

    return it->second;
}
}
