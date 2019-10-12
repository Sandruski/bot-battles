#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"

#include <array>
#include <memory>
#include <queue>
#include <unordered_map>

namespace sand {

//----------------------------------------------------------------------------------------------------
class EntityManager {
public:
    EntityManager();
    ~EntityManager();

    Entity AddEntity();
    bool RemoveEntity(Entity entity);

private:
    std::array<Signature, MAX_ENTITIES> m_signatures;
    std::unordered_map<Entity, U32> m_entitiesToSignatures;
    std::queue<Entity> m_availableEntities;
    U32 m_signaturesSize;
};
}

#endif
