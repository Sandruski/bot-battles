#include "NavigationSystem.h"

#include "ClientProxy.h"
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
bool NavigationSystem::Update()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        std::weak_ptr<InputComponent> inputComponent = g_gameServer->GetComponentManager().GetComponent<InputComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxyFromEntity(entity);
        if (clientProxy != nullptr) {
            const std::deque<Move>& unprocessedMoves = clientProxy->GetUnprocessedMoves();
            for (const Move& unprocessedMove : unprocessedMoves) {
                const InputComponent& unprocessedInput = unprocessedMove.GetInput();
                UpdateMovement(entity, unprocessedInput, *transformComponent.lock(), unprocessedMove.GetDt());
                inputComponent.lock()->Copy(unprocessedInput);
            }
        } else {
            UpdateMovement(entity, *inputComponent.lock(), *transformComponent.lock(), Time::GetInstance().GetDt());
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void NavigationSystem::UpdateMovement(Entity entity, const InputComponent& input, TransformComponent& transform, F32 dt) const
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
