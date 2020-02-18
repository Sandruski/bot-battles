#include "ComponentManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ComponentManager::ComponentManager()
    : m_componentArrays()
{
    m_componentArrays.fill(nullptr);
}

//----------------------------------------------------------------------------------------------------
void ComponentManager::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::ENTITY_REMOVED: {
        OnEntityRemoved(event.entity.entity);
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
bool ComponentManager::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
std::weak_ptr<Component> ComponentManager::AddComponent(ComponentType componentType, Entity entity)
{
    assert(componentType < ComponentType::COUNT && entity < INVALID_ENTITY);

    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    std::weak_ptr<Component> component = m_componentArrays.at(componentIndex)->AddComponent(entity);
    if (!component.expired()) {
        Event newEvent;
        newEvent.eventType = EventType::COMPONENT_ADDED;
        newEvent.component.componentType = componentType;
        newEvent.component.entity = entity;
        NotifyEvent(newEvent);
    }

    return component;
}

//----------------------------------------------------------------------------------------------------
std::weak_ptr<Component> ComponentManager::GetComponent(ComponentType componentType, Entity entity)
{
    assert(componentType < ComponentType::COUNT && entity < INVALID_ENTITY);

    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    return m_componentArrays.at(componentIndex)->GetComponent(entity);
}

//----------------------------------------------------------------------------------------------------
bool ComponentManager::RemoveComponent(ComponentType componentType, Entity entity)
{
    assert(componentType < ComponentType::COUNT && entity < INVALID_ENTITY);

    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    bool ret = m_componentArrays.at(componentIndex)->RemoveComponent(entity);
    if (ret) {
        Event newEvent;
        newEvent.eventType = EventType::COMPONENT_REMOVED;
        newEvent.component.componentType = componentType;
        newEvent.component.entity = entity;
        PushEvent(newEvent);
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
void ComponentManager::OnEntityRemoved(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    for (auto& componentArray : m_componentArrays) {
        componentArray->KillComponent(entity);
    }
}

//----------------------------------------------------------------------------------------------------
void ComponentManager::OnComponentRemoved(ComponentType componentType, Entity entity)
{
    assert(componentType < ComponentType::COUNT && entity < INVALID_ENTITY);

    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    m_componentArrays.at(componentIndex)->KillComponent(entity);
}
}
