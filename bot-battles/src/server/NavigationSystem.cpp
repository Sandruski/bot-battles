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

        PlayerID playerID = serverComponent.GetPlayerID(entity);
        std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        if (clientProxy != nullptr) {
            U32 moveCount = clientProxy->m_moves.GetMoveCount();
            for (U32 i = 0; i < moveCount; ++i) {
                const Move& move = clientProxy->m_moves.GetMove(i);
                const InputComponent& moveInputComponent = move.GetInput();
                UpdateMovement(entity, moveInputComponent, *transformComponent.lock(), move.GetDt());
                inputComponent.lock()->Copy(moveInputComponent);
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
    transform.Move(input, dt);

    Event newEvent;
    newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newEvent.component.entity = entity;
    newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::TRANSFORM_ALL);
    NotifyEvent(newEvent);
}
}
