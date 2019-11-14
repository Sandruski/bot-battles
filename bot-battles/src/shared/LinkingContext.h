#ifndef __LINKING_CONTEXT_H__
#define __LINKING_CONTEXT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class LinkingContext {
public:
    LinkingContext();
    ~LinkingContext();

    NetworkID AddEntity(Entity entity, NetworkID networkID = INVALID_NETWORK_ID);
    bool RemoveEntity(Entity entity);

    NetworkID GetNetworkID(Entity entity) const;
    Entity GetEntity(NetworkID networkID) const;
    const std::unordered_map<NetworkID, Entity>& GetNetworkIDToEntityMap() const;

private:
    std::unordered_map<NetworkID, Entity> m_networkIDToEntity;
    std::unordered_map<Entity, NetworkID> m_entityToNetworkID;
    std::queue<Entity> m_availableNetworkIDs;
};
}

#endif
