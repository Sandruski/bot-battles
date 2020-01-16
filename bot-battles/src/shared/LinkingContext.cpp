#include "LinkingContext.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
LinkingContext::LinkingContext()
{
    for (Entity i = 0; i < MAX_NETWORK_IDS; ++i) {
        m_availableNetworkIDs.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
NetworkID LinkingContext::AddEntity(Entity entity, NetworkID networkID)
{
    NetworkID existingNetworkID = GetNetworkID(entity);
    if (existingNetworkID != INVALID_NETWORK_ID) {
        WLOG("Entity %u already exists with the networkID %u", entity, existingNetworkID);
        return INVALID_NETWORK_ID;
    }

    NetworkID newNetworkID = networkID;
    if (newNetworkID == INVALID_NETWORK_ID) {
        newNetworkID = m_availableNetworkIDs.front();
        m_availableNetworkIDs.pop();
    }

    m_networkIDToEntity.insert(std::make_pair(newNetworkID, entity));
    m_entityToNetworkID.insert(std::make_pair(entity, newNetworkID));

    return newNetworkID;
}

//----------------------------------------------------------------------------------------------------
bool LinkingContext::RemoveEntity(Entity entity)
{
    NetworkID networkID = GetNetworkID(entity);
    if (networkID == INVALID_NETWORK_ID) {
        WLOG("Entity %u does not exist", entity);
        return false;
    }

    m_networkIDToEntity.erase(networkID);
    m_entityToNetworkID.erase(entity);

    m_availableNetworkIDs.push(networkID);

    return true;
}

//----------------------------------------------------------------------------------------------------
NetworkID LinkingContext::GetNetworkID(Entity entity) const
{
    auto it = m_entityToNetworkID.find(entity);
    if (it == m_entityToNetworkID.end()) {
        return INVALID_NETWORK_ID;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
Entity LinkingContext::GetEntity(NetworkID networkID) const
{
    auto it = m_networkIDToEntity.find(networkID);
    if (it == m_networkIDToEntity.end()) {
        return INVALID_ENTITY;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
const std::unordered_map<NetworkID, Entity>& LinkingContext::GetNetworkIDToEntityMap() const
{
    return m_networkIDToEntity;
}
}
