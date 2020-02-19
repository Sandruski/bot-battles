#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class EntityManager : public Subject, public Observer, public std::enable_shared_from_this<EntityManager> {
public:
    EntityManager();

    void OnNotify(const Event& event) override;

    bool PreUpdate();

    Entity AddEntity();
    Signature GetSignature(Entity entity);
    bool RemoveEntity(Entity entity);
    void ClearEntities();

private:
    bool KillEntity(Entity entity);

    void OnEntityRemoved(Entity entity);
    void OnComponentAdded(ComponentType componentType, Entity entity);
    void OnComponentRemoved(ComponentType componentType, Entity entity);

private:
    std::array<Signature, MAX_ENTITIES> m_signatures;
    std::unordered_map<Entity, U32> m_entitiesToSignatures;
    std::queue<Entity> m_availableEntities;
    U32 m_signaturesSize;
};
}

#endif
