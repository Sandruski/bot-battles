#include "NavigationSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
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

        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) == INVALID_NETWORK_ID) {
            continue;
        }

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isClientSidePrediction && clientComponent.m_isLastMovePending) {
                const Move& move = clientComponent.m_moves.GetLastMove();
                const InputComponent& inputComponent = move.GetInputComponent();
                F32 dt = move.GetDt();

                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);

                clientComponent.m_isLastMovePending = false;

                ILOG("CLIENTTT POSITION END: %f %f", transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y);
            }
        } else {
            if (clientComponent.m_isEntityInterpolation) {
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);

                //https://antriel.com/post/online-platformer-5/

                if (transformComponent.lock()->m_position != transformComponent.lock()->m_endPosition) {
                    F32 outOfSyncTime = time - transformComponent.lock()->m_outOfSyncTimestamp; // TODO: pick frame start time
                    F32 t = outOfSyncTime / ENTITY_INTERPOLATION_PERIOD;
                    ILOG("OUTOFSYNCTIME %f RTT %f T %f", outOfSyncTime, rtt, t);
                    //if (outOfSyncTime < rtt) {
                    transformComponent.lock()->m_position = Lerp(transformComponent.lock()->m_startPosition, transformComponent.lock()->m_endPosition, t <= 1.0f ? t : 1.0f);
                    //}
                    ILOG("DIFF POSITION. INITIAL: %f %f FINAL: %f %f CURRENT: %f %f",
                        transformComponent.lock()->m_startPosition.x, transformComponent.lock()->m_startPosition.y,
                        transformComponent.lock()->m_endPosition.x, transformComponent.lock()->m_endPosition.y,
                        transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y);
                } else {
                    transformComponent.lock()->m_outOfSyncTimestamp = 0.0f;
                    ILOG("SAME POSITION");
                }
            }
        }
    }

    return true;
}
}
