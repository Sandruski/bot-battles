#include "EntityManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
EntityManager::EntityManager()
    : m_signatures()
    , m_entitiesToSignatures()
    , m_availableEntities()
    , m_signaturesSize(0)
{
    m_signatures.fill(NULL);
    m_entitiesToSignatures.reserve(MAX_ENTITIES);

    for (Entity i = 0; i < MAX_ENTITIES; ++i) {
        m_availableEntities.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
void EntityManager::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::ENTITY_REMOVED: {
        OnEntityRemoved(event.entity.entity);
        break;
    }

    case EventType::COMPONENT_ADDED: {
        OnComponentAdded(event.component.componentType, event.component.entity);
        break;
    }

    case EventType::COMPONENT_REMOVED: {
        OnComponentRemoved(event.component.componentType, event.component.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
bool EntityManager::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
const Entity& EntityManager::AddEntity()
{
    Entity entity = m_availableEntities.front();
    m_availableEntities.pop();

    U32 signatureIndex = m_signaturesSize;

    m_entitiesToSignatures.insert(std::make_pair(entity, signatureIndex));

    ++m_signaturesSize;

    Event newEvent;
    newEvent.eventType = EventType::ENTITY_ADDED;
    newEvent.entity.entity = entity;
    NotifyEvent(newEvent);

    return GetEntity(entity);
}

//----------------------------------------------------------------------------------------------------
bool EntityManager::RemoveEntity(Entity entity)
{
    auto entityToSignature = m_entitiesToSignatures.find(entity);
    if (entityToSignature == m_entitiesToSignatures.end()) {
        WLOG("Entity %u does not exist", entity);
        return false;
    }

    Event newEvent;
    newEvent.eventType = EventType::ENTITY_REMOVED;
    newEvent.entity.entity = entity;
    PushEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
void EntityManager::ClearEntities()
{
    for (const auto& pair : m_entitiesToSignatures) {
        Entity entity = pair.first;
        RemoveEntity(entity);
    }
}

//----------------------------------------------------------------------------------------------------
const Signature& EntityManager::GetSignature(Entity entity)
{
    U32 signatureIndex = m_entitiesToSignatures.at(entity);
    return m_signatures.at(signatureIndex);
}

//----------------------------------------------------------------------------------------------------
const Entity& EntityManager::GetEntity(Entity entity)
{
    return m_entitiesToSignatures.find(entity)->first;
}

//----------------------------------------------------------------------------------------------------
bool EntityManager::KillEntity(Entity entity)
{
    auto entityToSignature = m_entitiesToSignatures.find(entity);
    if (entityToSignature == m_entitiesToSignatures.end()) {
        return false;
    }

    U32 signatureIndex = entityToSignature->second;
    U32 lastSignatureIndex = m_signaturesSize - 1;
    m_signatures.at(signatureIndex) = m_signatures.at(lastSignatureIndex);
    m_signatures.at(lastSignatureIndex) = NULL;

    auto lastEntityToSignature = std::find_if(m_entitiesToSignatures.begin(),
        m_entitiesToSignatures.end(),
        [lastSignatureIndex](const auto& value) {
            return value.second == lastSignatureIndex;
        });
    assert(lastEntityToSignature != m_entitiesToSignatures.end());

    m_entitiesToSignatures.at(lastEntityToSignature->first) = signatureIndex;
    m_entitiesToSignatures.erase(entity);

    m_availableEntities.push(entity);

    --m_signaturesSize;

    return true;
}

//----------------------------------------------------------------------------------------------------
void EntityManager::OnEntityRemoved(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    KillEntity(entity);
}

//----------------------------------------------------------------------------------------------------
void EntityManager::OnComponentAdded(ComponentType componentType, Entity entity)
{
    assert(componentType < ComponentType::COUNT && entity < INVALID_ENTITY);

    U32 signatureIndex = m_entitiesToSignatures.at(entity);
    Signature& signature = m_signatures.at(signatureIndex);
    signature |= 1 << static_cast<std::size_t>(componentType);

    Event newEvent;
    newEvent.eventType = EventType::ENTITY_SIGNATURE_CHANGED;
    newEvent.entity.entity = entity;
    newEvent.entity.signature = signature;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void EntityManager::OnComponentRemoved(ComponentType componentType, Entity entity)
{
    assert(componentType < ComponentType::COUNT && entity < INVALID_ENTITY);

    U32 signatureIndex = m_entitiesToSignatures.at(entity);
    Signature& signature = m_signatures.at(signatureIndex);
    signature &= ~(1 << static_cast<std::size_t>(componentType));

    Event newEvent;
    newEvent.eventType = EventType::ENTITY_SIGNATURE_CHANGED;
    newEvent.entity.entity = entity;
    newEvent.entity.signature = signature;
    NotifyEvent(newEvent);
}
}
