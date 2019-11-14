#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class EntityManager : public Subject, public Observer {
public:
    EntityManager();
    ~EntityManager();

    bool PreUpdate();

    Entity AddEntity();
    bool RemoveEntity(Entity entity);

    const Signature& GetSignature(Entity entity);

    void OnNotify(const Event& event) override;

private:
    void KillEntity(Entity entity);

private:
    std::array<Signature, MAX_ENTITIES> m_signatures;
    std::unordered_map<Entity, U32> m_entitiesToSignatures;
    std::queue<Entity> m_availableEntities;
    U32 m_signaturesSize;
};
}

#endif
