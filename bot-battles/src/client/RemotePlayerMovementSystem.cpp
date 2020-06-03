#include "RemotePlayerMovementSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "RigidbodyComponent.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RemotePlayerMovementSystem::RemotePlayerMovementSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
    m_signature |= 1 << static_cast<U16>(ComponentType::REMOTE_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool RemotePlayerMovementSystem::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.m_isEntityInterpolation) {
        return true;
    }

    if (clientComponent.m_frameBuffer.Count() >= 2) {
        clientComponent.m_interpolationFromFrame = clientComponent.m_frameBuffer.GetFirst().GetFrame();
        clientComponent.m_interpolationToFrame = clientComponent.m_frameBuffer.GetSecond().GetFrame();
        F32 startFrameTime = MyTime::GetInstance().GetStartFrameTime();
        F32 outOfSyncTime = startFrameTime - clientComponent.m_frameBuffer.GetSecond().GetTimestamp();
        clientComponent.m_interpolationPercentage = outOfSyncTime / clientComponent.m_entityInterpolationPeriod;
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

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

        if (!transformComponent.lock()->m_transformBuffer.IsEmpty()) {
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

            if (transformComponent.lock()->m_transformBuffer.Count() >= 2) {
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
                    glm::vec2 interpolatedPosition = Lerp(fromTransform.m_position, toTransform.m_position, clientComponent.m_interpolationPercentage);
                    F32 interpolatedRotation = Lerp(fromTransform.m_rotation, toTransform.m_rotation, clientComponent.m_interpolationPercentage);
                    rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(interpolatedPosition.x), PIXELS_TO_METERS(interpolatedPosition.y)), glm::radians(interpolatedRotation));

                    b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                    transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                    float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                    transformComponent.lock()->m_rotation = glm::degrees(physicsRotation);

                    Event newComponentEvent;
                    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                    newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION) | static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
                    newComponentEvent.component.entity = entity;
                    NotifyEvent(newComponentEvent);
                }
            }

            if (clientComponent.m_interpolationPercentage == 1.0f) {
                if (isFoundFrom) {
                    transformComponent.lock()->m_transformBuffer.Remove(indexFrom);
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RemotePlayerMovementSystem::DebugRender()
{
    OPTICK_EVENT();

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        DebugDraw(transformComponent);
    }

    return true;
}
}
