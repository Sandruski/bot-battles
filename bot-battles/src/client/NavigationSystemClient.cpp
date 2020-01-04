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
    if (!clientComponent.m_isClientSidePrediction) {
        return true;
    }

    MoveComponent& moveComponent = g_gameClient->GetMoveComponent();
    if (!moveComponent.m_isLastMovePending) {
        return true;
    }

    for (auto& entity : m_entities) {

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {

            const Move& move = moveComponent.m_moves.GetLastMove();
            const InputComponent& inputComponent = move.GetInput();
            F32 dt = move.GetDt();

            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);

            moveComponent.m_isLastMovePending = false;

            break;
        }
    }

    return true;
}
}
