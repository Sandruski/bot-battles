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
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::PLAY) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.m_frameBuffer.Count() >= 2) {
        clientComponent.m_interpolationFromFrame = clientComponent.m_frameBuffer.GetFirst().GetFrame();
        clientComponent.m_interpolationToFrame = clientComponent.m_frameBuffer.GetSecond().GetFrame();
        F32 startFrameTime = MyTime::GetInstance().GetStartFrameTime();
        F32 outOfSyncTime = startFrameTime - clientComponent.m_frameBuffer.GetSecond().GetTimestamp();
        clientComponent.m_interpolationPercentage = outOfSyncTime / ENTITY_INTERPOLATION_PERIOD;
        if (clientComponent.m_interpolationPercentage > 1.0f) {
            clientComponent.m_interpolationPercentage = 1.0f;
        }

        if (clientComponent.m_interpolationPercentage == 1.0f) {
            clientComponent.m_frameBuffer.RemoveFirst();
        }
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (clientComponent.m_isEntityInterpolation) {
            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            if (transformComponent.lock()->m_transformBuffer.Count() >= 2) {
                U32 indexFrom = transformComponent.lock()->m_transformBuffer.m_front;
                bool isFoundFrom = false;
                while (indexFrom < transformComponent.lock()->m_transformBuffer.m_back) {
                    const Transform& transform = transformComponent.lock()->m_transformBuffer.Get(indexFrom);
                    if (transform.GetFrame() == clientComponent.m_interpolationFromFrame) {
                        isFoundFrom = true;
                        break;
                    }
                    ++indexFrom;
                }

                U32 indexTo = transformComponent.lock()->m_transformBuffer.m_front;
                bool isFoundTo = false;
                while (indexTo < transformComponent.lock()->m_transformBuffer.m_back) {
                    const Transform& transform = transformComponent.lock()->m_transformBuffer.Get(indexTo);
                    if (transform.GetFrame() == clientComponent.m_interpolationToFrame) {
                        isFoundTo = true;
                        break;
                    }
                    ++indexTo;
                }

                if (isFoundFrom && isFoundTo) {
                    Transform fromTransform = transformComponent.lock()->m_transformBuffer.Get(indexFrom);
                    Transform toTransform = transformComponent.lock()->m_transformBuffer.Get(indexTo);
                    transformComponent.lock()->m_position = Lerp(fromTransform.m_position, toTransform.m_position, clientComponent.m_interpolationPercentage);
                    transformComponent.lock()->m_rotation = Lerp(fromTransform.m_rotation, toTransform.m_rotation, clientComponent.m_interpolationPercentage);
                    if (clientComponent.m_interpolationPercentage == 1.0f) {
                        transformComponent.lock()->m_transformBuffer.RemoveFirst();
                    }
                }
            }
        }
    }

    return true;
}
}
