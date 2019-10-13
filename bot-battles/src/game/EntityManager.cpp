#include "EntityManager.h"

#include "Game.h"
#include "ComponentManager.h"
#include "Events.h"

#include "Log.h"

#include <cassert>

namespace sand {

//----------------------------------------------------------------------------------------------------
EntityManager::EntityManager()
    : m_signatures()
    , m_entitiesToSignatures()
    , m_availableEntities()
    , m_signaturesSize(0)
{
    memset(&m_signatures, NULL, MAX_ENTITIES);

    for (Entity i = 0; i < MAX_ENTITIES; ++i) {
        m_availableEntities.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
EntityManager::~EntityManager()
{
}

//----------------------------------------------------------------------------------------------------
bool EntityManager::PreUpdate(F32 /*dt*/)
{
	NotifyAll();

	return true;
}

//----------------------------------------------------------------------------------------------------
Entity EntityManager::AddEntity()
{
    Entity entity = m_availableEntities.front();
    m_availableEntities.pop();

    U32 signatureIndex = m_signaturesSize;
    ++m_signaturesSize;

    m_entitiesToSignatures[entity] = signatureIndex;

    // TODO: add default Transform component
    //g_game->GetComponentManager().AddComponent<TransformComponent>(entity);

    return entity;
}

//----------------------------------------------------------------------------------------------------
bool EntityManager::RemoveEntity(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    auto entityToSignature = m_entitiesToSignatures.find(entity);
    if (entityToSignature == m_entitiesToSignatures.end()) {
        LOG("The entity %u does not exist!", entity);
        return false;
    }

    // TODO: remove all components

    U32 signatureIndex = entityToSignature->second;
    U32 lastSignatureIndex = m_signaturesSize - 1;
    m_signatures[signatureIndex] = m_signatures[lastSignatureIndex];
    m_signatures[lastSignatureIndex] = NULL;

    auto lastEntityToSignature = std::find_if(m_entitiesToSignatures.begin(),
        m_entitiesToSignatures.end(),
        [lastSignatureIndex](const auto& value) {
            return value.second == lastSignatureIndex;
        });
    assert(lastEntityToSignature != m_entitiesToSignatures.end());

    m_entitiesToSignatures[lastEntityToSignature->first] = signatureIndex;
    m_entitiesToSignatures.erase(entity);

    m_availableEntities.push(entity);

    --m_signaturesSize;

	Event event;
	event.type = EventType::ENTITY_REMOVED;
	event.entity.entity = entity;
	PushEvent(event);

    return true;
}
}
