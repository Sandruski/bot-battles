#include "SightSystemServer.h"

#include "BotComponent.h"
#include "ComponentManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "ShaderResource.h"
#include "SightComponent.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightSystemServer::SightSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SIGHT);
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();
    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameServer->GetComponentManager().GetComponent<SightComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !sightComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec2 center = transformComponent.lock()->m_position;
        glm::vec2 extents = glm::vec2(sightComponent.lock()->m_distance, sightComponent.lock()->m_distance);
        std::vector<Entity> seenEntities;
        const bool isOverlap = physicsComponent.Overlap(center, extents, seenEntities);
        if (!isOverlap) {
            continue;
        }

        glm::vec2 direction = transformComponent.lock()->GetDirection();
        for (auto& seenEntity : seenEntities) {
            std::weak_ptr<TransformComponent> seenTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(seenEntity);
            if (!seenTransformComponent.lock()->m_isEnabled) {
                continue;
            }

            const bool isInFoV = IsInFoV(transformComponent.lock()->m_position, seenTransformComponent.lock()->m_position, direction, sightComponent.lock()->m_angle);
            if (!isInFoV) {
                if (sightComponent.lock()->m_target < INVALID_ENTITY) {
                    sightComponent.lock()->m_target = INVALID_ENTITY;
                    Event newSightEvent;
                    newSightEvent.eventType = EventType::SEEN_BOT_EXIT;
                    PushEvent(newSightEvent);
                }
                continue;
            }

            const bool isInLoS = IsInLoS(physicsComponent, transformComponent.lock()->m_position, seenTransformComponent.lock()->m_position, sightComponent.lock()->m_distance);
            if (!isInLoS) {
                if (sightComponent.lock()->m_target < INVALID_ENTITY) {
                    sightComponent.lock()->m_target = INVALID_ENTITY;
                    Event newSightEvent;
                    newSightEvent.eventType = EventType::SEEN_BOT_EXIT;
                    PushEvent(newSightEvent);
                }
                continue;
            }

            if (sightComponent.lock()->m_target >= INVALID_ENTITY) {
                sightComponent.lock()->m_target = entity;
                Event newSightEvent;
                newSightEvent.eventType = EventType::SEEN_BOT_ENTER;
                PushEvent(newSightEvent);
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::DebugRender()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameServer->GetComponentManager().GetComponent<SightComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !sightComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation + sightComponent.lock()->m_angle / 2.0f;
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 0.0f);
        glm::vec4 color = Red;
        color.a = 0.5f;
        rendererComponent.DrawCircle(position, rotation, scale, sightComponent.lock()->m_angle, sightComponent.lock()->m_distance, color, true);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::IsInFoV(glm::vec2 position, glm::vec2 targetPosition, glm::vec2 direction, F32 angle) const
{
    glm::vec2 vectorToTarget = targetPosition - position;
    glm::vec2 directionToTarget = glm::normalize(vectorToTarget);
    F32 angleToTarget = glm::angle(direction, directionToTarget);
    return angleToTarget <= angle;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::IsInLoS(PhysicsComponent& physicsComponent, glm::vec2 position, glm::vec2 targetPosition, F32 distance) const
{
    glm::vec2 vectorToTarget = targetPosition - position;
    glm::vec2 directionToTarget = glm::normalize(vectorToTarget);
    glm::vec2 distanceToTarget = directionToTarget * distance;

    PhysicsComponent::RaycastHit raycastHit;
    if (physicsComponent.Raycast(position, distanceToTarget, raycastHit)) {
        std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(raycastHit.m_entity);
        if (!botComponent.expired()) {
            return true;
        }
    }

    return false;
}
}
