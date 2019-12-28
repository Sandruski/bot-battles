#include "NavigationSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "InputComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
NavigationSystem::NavigationSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::INPUT);
}

//----------------------------------------------------------------------------------------------------
NavigationSystem::~NavigationSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystem::Update()
{
    for (auto& entity : m_entities) {
        std::shared_ptr<InputComponent> inputComponent = g_gameServer->GetComponentManager().GetComponent<InputComponent>(entity);
        std::shared_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        UpdateMovement(entity, *inputComponent, *transformComponent, Time::GetInstance().GetDt()); // TODO: use client dt
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void NavigationSystem::UpdateMovement(Entity entity, InputComponent& input, TransformComponent& transform, F32 dt) const
{
    transform.m_position.x += input.m_acceleration.x * dt;
    transform.m_position.y += input.m_acceleration.y * dt;
    transform.m_rotation += input.m_angularAcceleration * dt;

    Event newEvent;
    newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newEvent.component.entity = entity;
    newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::TRANSFORM_ALL);
    NotifyEvent(newEvent);
}
}
