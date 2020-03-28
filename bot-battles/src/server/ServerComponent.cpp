#include "ServerComponent.h"

#include "ClientProxy.h"
#include "SocketAddress.h"
#include "TCPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ServerComponent::ServerComponent()
    : m_UDPSocket()
    , m_TCPListenSocket()
    , m_TCPSockets()
    , m_socketAddress()
    , m_port()
    , m_playerIDToClientProxy()
    , m_entityToPlayerID()
    , m_availablePlayerIDs()
    , m_map()
    , m_isServerRewind(false)
    , m_connect(false)
{
    m_playerIDToClientProxy.reserve(MAX_PLAYER_IDS);
    m_entityToPlayerID.reserve(MAX_PLAYER_IDS);

    for (PlayerID i = 0; i < MAX_PLAYER_IDS; ++i) {
        m_availablePlayerIDs.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("port"));
    assert(value["port"].IsString());
    m_port = value["port"].GetString();

    assert(value.HasMember("defaultMap"));
    assert(value["defaultMap"].IsString());
    m_map = value["defaultMap"].GetString();

    assert(value.HasMember("serverRewind"));
    assert(value["serverRewind"].IsBool());
    m_isServerRewind = value["serverRewind"].GetBool();
}

//----------------------------------------------------------------------------------------------------
PlayerID ServerComponent::AddPlayer(const SocketAddress& socketAddress, const char* name)
{
    PlayerID playerID = GetPlayerID(socketAddress);
    if (playerID < INVALID_PLAYER_ID) {
        WLOG("Player with socket address %s could not be added", socketAddress.GetName());
        return playerID;
    }

    if (m_availablePlayerIDs.empty()) {
        WLOG("Player with socket address %s could not be added", socketAddress.GetName());
        return playerID;
    }

    playerID = m_availablePlayerIDs.front();
    m_availablePlayerIDs.pop();

    std::shared_ptr<ClientProxy> clientProxy = std::make_shared<ClientProxy>(socketAddress, name);
    m_playerIDToClientProxy.insert(std::make_pair(playerID, clientProxy));

    return playerID;
}

//----------------------------------------------------------------------------------------------------
bool ServerComponent::RemovePlayer(PlayerID playerID)
{
    std::weak_ptr<ClientProxy> clientProxy = GetClientProxy(playerID);
    if (clientProxy.expired()) {
        WLOG("Player %u could not be removed", playerID);
        return false;
    }

    m_playerIDToClientProxy.erase(playerID);

    m_availablePlayerIDs.push(playerID);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerComponent::AddEntity(Entity entity, PlayerID playerID)
{
    PlayerID existingPlayerID = GetPlayerID(entity);
    if (existingPlayerID < INVALID_PLAYER_ID) {
        WLOG("Entity %u could not be added", entity);
        return false;
    }

    return m_entityToPlayerID.insert(std::make_pair(entity, playerID)).second;
}

//----------------------------------------------------------------------------------------------------
bool ServerComponent::RemoveEntity(Entity entity)
{
    PlayerID playerID = GetPlayerID(entity);
    if (playerID >= INVALID_PLAYER_ID) {
        WLOG("Entity %u could not be removed", entity);
        return false;
    }

    m_entityToPlayerID.erase(entity);

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
std::weak_ptr<ClientProxy> ServerComponent::GetClientProxy(PlayerID playerID) const
{
    auto it = m_playerIDToClientProxy.find(playerID);
    if (it == m_playerIDToClientProxy.end()) {
        return std::weak_ptr<ClientProxy>();
    }

    return std::weak_ptr<ClientProxy>(it->second);
}

//----------------------------------------------------------------------------------------------------
PlayerID ServerComponent::GetPlayerID(Entity entity) const
{
    auto it = m_entityToPlayerID.find(entity);
    if (it == m_entityToPlayerID.end()) {
        return INVALID_PLAYER_ID;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
Entity ServerComponent::GetEntity(PlayerID playerID) const
{
    for (const auto& pair : m_entityToPlayerID) {
        if (pair.second == playerID) {
            return pair.first;
        }
    }

    return INVALID_ENTITY;
}

//----------------------------------------------------------------------------------------------------
U32 ServerComponent::GetEntityCount() const
{
    return m_entityToPlayerID.size();
}

//----------------------------------------------------------------------------------------------------
const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& ServerComponent::GetPlayerIDToClientProxyMap() const
{
    return m_playerIDToClientProxy;
}

//----------------------------------------------------------------------------------------------------
std::weak_ptr<TCPSocket> ServerComponent::GetTCPSocket(const SocketAddress& socketAddress) const
{
    for (const auto& TCPSock : m_TCPSockets) {
        if (TCPSock->GetRemoteSocketAddress() == socketAddress) {
            return std::weak_ptr<TCPSocket>(TCPSock);
        }
    }

    return std::weak_ptr<TCPSocket>();
}
}
