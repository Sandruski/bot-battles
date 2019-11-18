#include "NavigationSystem.h"

#include "ComponentManager.h"
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
        std::shared_ptr<InputComponent> input = g_gameServer->GetComponentManager().GetComponent<InputComponent>(entity);
        std::shared_ptr<TransformComponent> transform = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        UpdateMovement(*input, *transform, Time::GetInstance().GetDt()); // TODO: use client dt
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void NavigationSystem::UpdateMovement(InputComponent& input, TransformComponent& transform, F32 /*dt*/) const
{
    transform.m_position += input.m_acceleration;
    transform.m_rotation += input.m_angularAcceleration;
}
}
