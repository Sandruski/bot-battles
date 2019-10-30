#include "LinkingContext.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
LinkingContext::LinkingContext()
{
    for (Entity i = 0; i < MAX_ENTITIES; ++i) { // TODO: should this be MAX_ENTITIES or MAX_NETWORK_IDs?
        m_availableNetworkIDs.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
LinkingContext::~LinkingContext()
{
}

//----------------------------------------------------------------------------------------------------
NetworkID LinkingContext::GetNetworkID(Entity entity) const
{
    assert(entity != INVALID_ENTITY);

    auto it = m_entityToNetworkID.find(entity);
    if (it == m_entityToNetworkID.end()) {
        WLOG("The entity %u has no associated networkID", entity);
        return INVALID_NETWORK_ID;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
Entity LinkingContext::GetEntity(NetworkID networkID) const
{
    assert(networkID != INVALID_NETWORK_ID);

    auto it = m_networkIDToEntity.find(networkID);
    if (it == m_networkIDToEntity.end()) {
        WLOG("The networkID %u has no associated entity", networkID);
        return INVALID_ENTITY;
    }

    return it->second;
}

//----------------------------------------------------------------------------------------------------
NetworkID LinkingContext::AddEntity(Entity entity, NetworkID networkID)
{
    assert(entity != INVALID_ENTITY);

    NetworkID existingNetworkID = GetNetworkID(entity);
    if (existingNetworkID != INVALID_NETWORK_ID) {
        WLOG("The entity %u already exists with the networkID %u", entity, existingNetworkID);
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
    assert(entity != INVALID_ENTITY);

    NetworkID networkID = GetNetworkID(entity);
    if (networkID == INVALID_NETWORK_ID) {
        WLOG("The entity %u does not exist", entity);
        return false;
    }

    m_networkIDToEntity.erase(networkID);
    m_entityToNetworkID.erase(entity);

    m_availableNetworkIDs.push(networkID);

    return true;
}
}
