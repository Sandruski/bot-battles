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
