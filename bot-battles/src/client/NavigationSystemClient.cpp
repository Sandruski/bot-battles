#include "NavigationSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "Interpolation.h"
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
    F32 frameStartTime = Time::GetInstance().GetStartFrameTime();

    if (clientComponent.m_isEntityInterpolation) {
        if (clientComponent.m_frameBuffer.Count() >= 2) {
            clientComponent.m_interpolationFromFrame = clientComponent.m_frameBuffer.GetFirst().GetFrame();
            clientComponent.m_interpolationToFrame = clientComponent.m_frameBuffer.GetSecond().GetFrame();
            F32 outOfSyncTime = frameStartTime - clientComponent.m_frameBuffer.GetSecond().GetTimestamp();
            clientComponent.m_interpolationPercentage = outOfSyncTime / ENTITY_INTERPOLATION_PERIOD;
            if (clientComponent.m_interpolationPercentage > 1.0f) {
                clientComponent.m_interpolationPercentage = 1.0f;
            }

            if (clientComponent.m_interpolationPercentage == 1.0f) {
                clientComponent.m_frameBuffer.RemoveFirst();
            }
        }
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) == INVALID_NETWORK_ID) {
            continue;
        }

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isClientPrediction) {
                if (clientComponent.m_isLastMovePending) {
                    const Input& input = clientComponent.m_inputBuffer.GetLast();
                    const InputComponent& inputComponent = input.GetInputComponent();
                    F32 dt = input.GetDt();

                    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                    transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);
                    Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
                    transformComponent.lock()->m_inputTransformBuffer.Add(transform);
                }
            }
        } else {
            if (clientComponent.m_isEntityInterpolation) {
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                if (transformComponent.lock()->m_transformBuffer.Count() >= 2) {
                    const Transform& fromTransform = transformComponent.lock()->m_transformBuffer.GetFirst();
                    const Transform& toTransform = transformComponent.lock()->m_transformBuffer.GetSecond();
                    transformComponent.lock()->m_position = Lerp(fromTransform.m_position, toTransform.m_position, clientComponent.m_interpolationPercentage);
                    transformComponent.lock()->m_rotation = Lerp(fromTransform.m_rotation, toTransform.m_rotation, clientComponent.m_interpolationPercentage);

                    if (clientComponent.m_interpolationPercentage == 1.0f) {
                        transformComponent.lock()->m_transformBuffer.RemoveFirst();
                    }
                }
            }
        }
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) == INVALID_NETWORK_ID) {
            continue;
        }

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isLastMovePending) {
                Input& input = clientComponent.m_inputBuffer.GetLast();
                input.m_interpolationFromFrame = clientComponent.m_interpolationFromFrame;
                input.m_interpolationToFrame = clientComponent.m_interpolationToFrame;
                input.m_interpolationPercentage = clientComponent.m_interpolationPercentage;
            }

            break;
        }
    }

    return true;
}
}
