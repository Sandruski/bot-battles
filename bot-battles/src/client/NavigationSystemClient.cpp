#include "NavigationSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "MoveComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
NavigationSystemClient::NavigationSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::INPUT);
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystemClient::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    MoveComponent& moveComponent = g_gameClient->GetMoveComponent();

    for (auto& entity : m_entities) {

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {

            if (moveComponent.m_isLastMovePending) {

                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);

                const Move& move = moveComponent.m_moves.GetLastMove();
                const InputComponent& moveInputComponent = move.GetInput();
                F32 dt = move.GetDt();
                transformComponent.lock()->UpdatePosition(moveInputComponent.m_acceleration, dt);
                transformComponent.lock()->UpdateRotation(moveInputComponent.m_angularAcceleration, dt);

                moveComponent.m_isLastMovePending = false;
            }
        } else {
            // TODO: what is this?
            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);

            F32 dt = Time::GetInstance().GetDt();
            transformComponent.lock()->UpdatePosition(inputComponent.lock()->m_acceleration, dt);
            transformComponent.lock()->UpdateRotation(inputComponent.lock()->m_angularAcceleration, dt);
        }
    }

    return true;
}
}
