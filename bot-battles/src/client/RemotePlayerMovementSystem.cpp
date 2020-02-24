#include "RemotePlayerMovementSystem.h"

#include "ComponentManager.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RemotePlayerMovementSystem::RemotePlayerMovementSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::REMOTE_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool RemotePlayerMovementSystem::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    const bool isConnected = clientComponent.IsConnected();
    const bool hasEntity = clientComponent.m_entity < INVALID_ENTITY;
    if (!isConnected || !hasEntity) {
        return true;
    }

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phaseType != PhaseType::PLAY) {
        return true;
    }

    if (clientComponent.m_isEntityInterpolation) {
        if (clientComponent.m_frameBuffer.Count() >= 2) {
            clientComponent.m_interpolationFromFrame = clientComponent.m_frameBuffer.GetFirst().GetFrame();
            clientComponent.m_interpolationToFrame = clientComponent.m_frameBuffer.GetSecond().GetFrame();
            F32 startFrameTime = Time::GetInstance().GetStartFrameTime();
            F32 outOfSyncTime = startFrameTime - clientComponent.m_frameBuffer.GetSecond().GetTimestamp();
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
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (clientComponent.m_isEntityInterpolation) {
            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            if (transformComponent.lock()->m_transformBuffer.Count() >= 2) {
                bool interpolate1 = false;
                Transform fromTransform;
                for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                    Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                    if (t.GetFrame() == clientComponent.m_interpolationFromFrame) {
                        fromTransform = t;
                        interpolate1 = true;
                        break;
                    }
                }
                bool interpolate2 = false;
                Transform toTransform;
                for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                    Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                    if (t.GetFrame() == clientComponent.m_interpolationToFrame) {
                        toTransform = t;
                        interpolate2 = true;
                        break;
                    }
                }

                if (interpolate1 && interpolate2) {
                    transformComponent.lock()->m_position = Lerp(fromTransform.m_position, toTransform.m_position, clientComponent.m_interpolationPercentage);
                    transformComponent.lock()->m_rotation = Lerp(fromTransform.m_rotation, toTransform.m_rotation, clientComponent.m_interpolationPercentage);
                    ILOG("From trans %u position %f %f, to trans %u position %f %f", clientComponent.m_interpolationFromFrame, fromTransform.m_position.x, fromTransform.m_position.y,
                        clientComponent.m_interpolationToFrame, toTransform.m_position.x, toTransform.m_position.y);
                }

                if (clientComponent.m_interpolationPercentage == 1.0f) {
                    transformComponent.lock()->m_transformBuffer.RemoveFirst();
                }
            }
        }
    }

    return true;
}
}
