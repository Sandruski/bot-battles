#include "EntityManager.h"

#include "Events.h"

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
bool EntityManager::PreUpdate()
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

    return entity;
}

//----------------------------------------------------------------------------------------------------
bool EntityManager::RemoveEntity(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    auto entityToSignature = m_entitiesToSignatures.find(entity);
    if (entityToSignature == m_entitiesToSignatures.end()) {
        WLOG("The entity %u does not exist!", entity);
        return false;
    }

    Event newEvent;
    newEvent.eventType = EventType::ENTITY_REMOVED;
    newEvent.entity.entity = entity;
    PushEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
const Signature& EntityManager::GetSignature(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    U32 signatureIndex = m_entitiesToSignatures.at(entity);
    return m_signatures[signatureIndex];
}

//----------------------------------------------------------------------------------------------------
void EntityManager::OnNotify(const Event& event)
{
    switch (event.eventType) {
    case EventType::ENTITY_REMOVED: {
        KillEntity(event.entity.entity);

        break;
    }

    case EventType::COMPONENT_ADDED: {
        U32 signatureIndex = m_entitiesToSignatures[event.component.entity];
        m_signatures[signatureIndex] |= 1 << static_cast<std::size_t>(event.component.componentType);

        Event newEvent;
        newEvent.eventType = EventType::ENTITY_SIGNATURE_CHANGED;
        newEvent.entity.entity = event.component.entity;
        newEvent.entity.signature = m_signatures[signatureIndex];
        PushEvent(newEvent);

        break;
    }

    case EventType::COMPONENT_REMOVED: {
        U32 signatureIndex = m_entitiesToSignatures[event.component.entity];
        m_signatures[signatureIndex] &= ~(1 << static_cast<std::size_t>(event.component.componentType));

        Event newEvent;
        newEvent.eventType = EventType::ENTITY_SIGNATURE_CHANGED;
        newEvent.entity.entity = event.component.entity;
        newEvent.entity.signature = m_signatures[signatureIndex];
        PushEvent(newEvent);

        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void EntityManager::KillEntity(Entity entity)
{
    U32 signatureIndex = m_entitiesToSignatures[entity];
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
}
}
