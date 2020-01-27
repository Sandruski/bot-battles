#include "NavigationSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
NavigationSystemClient::NavigationSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystemClient::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    F32 rtt = clientComponent.m_RTT;
    F32 time = Time::GetInstance().GetTime();

    for (auto& entity : m_entities) {

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isClientSidePrediction && clientComponent.m_isLastMovePending)
            {
                const Move& move = clientComponent.m_moves.GetLastMove();
                const InputComponent& inputComponent = move.GetInputComponent();
                F32 dt = move.GetDt();

                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);

                clientComponent.m_isLastMovePending = false;

                ILOG("CLIENTTT POSITION END: %f %f", transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y);
            }
        }
        else {
            if (clientComponent.m_isEntityInterpolation) 
            {
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);

                if (transformComponent.lock()->m_position != transformComponent.lock()->m_lastPosition) {

                    F32 outOfSyncTime = time - transformComponent.lock()->m_outOfSyncTimestamp;
                    if (outOfSyncTime < rtt) {
                        F32 t = outOfSyncTime / rtt;
                        transformComponent.lock()->m_position = Lerp(transformComponent.lock()->m_position, transformComponent.lock()->m_lastPosition, t);
                    }
                }
                else
                {
                    transformComponent.lock()->m_outOfSyncTimestamp = 0.0f;
                }
            }
        }
    }

    return true;
}
}
