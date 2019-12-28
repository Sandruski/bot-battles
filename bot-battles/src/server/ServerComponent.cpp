#include "ServerComponent.h"

#include "ClientProxy.h"
#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ServerComponent::ServerComponent()
    : m_socket(nullptr)
    , m_socketAddress(nullptr)
    , m_receivedPackets()
    , m_playerIDToClientProxy()
    , m_availablePlayerIDs()
{
    m_playerIDToClientProxy.reserve(MAX_PLAYER_IDS);

    for (PlayerID i = 0; i < MAX_PLAYER_IDS; ++i) {
        m_availablePlayerIDs.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
PlayerID ServerComponent::AddPlayer(const SocketAddress& socketAddress, const char* name)
{
    PlayerID playerID = GetPlayerID(socketAddress);
    if (playerID != INVALID_PLAYER_ID) {
        std::shared_ptr<ClientProxy> clientProxy = GetClientProxyFromPlayerID(playerID);
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
bool ServerComponent::RemovePlayer(PlayerID playerID)
{
    std::size_t ret = m_playerIDToClientProxy.erase(playerID);
    if (ret == 0) {
        WLOG("Player %u could not be removed", playerID);
        return false;
    }

    m_availablePlayerIDs.push(playerID);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerComponent::AddEntity(Entity entity, PlayerID playerID)
{
    std::shared_ptr<ClientProxy> clientProxy = GetClientProxyFromPlayerID(playerID);
    if (clientProxy == nullptr) {
        return false;
    }

    m_entityToClientProxy.insert(std::make_pair(entity, clientProxy));

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerComponent::RemoveEntity(Entity entity)
{
    std::size_t ret = m_entityToClientProxy.erase(entity);
    if (ret == 0) {
        WLOG("Entity %u could not be removed", entity);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
PlayerID ServerComponent::GetPlayerID(const SocketAddress& socketAddress) const
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
Entity ServerComponent::GetEntity(const SocketAddress& socketAddress) const
{
    for (const auto& pair : m_entityToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        if (clientProxy->GetSocketAddress() == socketAddress) {
            return pair.first;
        }
    }

    return INVALID_ENTITY;
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<ClientProxy> ServerComponent::GetClientProxyFromPlayerID(PlayerID playerID) const
{
    auto it = m_playerIDToClientProxy.find(playerID);
    if (it == m_playerIDToClientProxy.end()) {
        WLOG("Player %u is not registered", playerID);
        return nullptr;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<ClientProxy> ServerComponent::GetClientProxyFromEntity(Entity entity) const
{
    auto it = m_entityToClientProxy.find(entity);
    if (it == m_entityToClientProxy.end()) {
        WLOG("Entity %u is not registered", entity);
        return nullptr;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& ServerComponent::GetPlayerIDToClientProxyMap()
{
    return m_playerIDToClientProxy;
}
}
