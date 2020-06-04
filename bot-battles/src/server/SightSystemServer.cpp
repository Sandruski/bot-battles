#include "SightSystemServer.h"

#include "BotComponent.h"
#include "ColliderComponent.h"
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
#include "SpriteComponent.h"
#include "State.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightSystemServer::SightSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::SIGHT);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
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

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    std::weak_ptr<PhysicsComponent> physicsComponent = g_gameServer->GetPhysicsComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameServer->GetComponentManager().GetComponent<SightComponent>(entity);

        std::vector<Entity> seenEntities;

        glm::vec2 center = transformComponent.lock()->m_position;
        glm::vec2 extents = glm::vec2(sightComponent.lock()->m_distance, sightComponent.lock()->m_distance);
        std::vector<Entity> overlapEntities;
        const bool isOverlap = physicsComponent.lock()->Overlap(center, extents, overlapEntities);
        if (isOverlap) {
            glm::vec2 direction = transformComponent.lock()->GetDirection();
            for (const auto& overlapEntity : overlapEntities) {
                NetworkID networkID = linkingContext.GetNetworkID(overlapEntity);
                if (networkID >= INVALID_NETWORK_ID) {
                    continue;
                }

                if (overlapEntity == entity) {
                    seenEntities.emplace_back(overlapEntity);
                    continue;
                }

                std::weak_ptr<TransformComponent> overlapTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(overlapEntity);
                const bool isInLoS = IsInLoS(physicsComponent, transformComponent.lock()->m_position, overlapTransformComponent.lock()->m_position, sightComponent.lock()->m_distance, overlapEntity);
                if (isInLoS) {
                    const bool isInFoV = IsInFoV(transformComponent.lock()->m_position, overlapTransformComponent.lock()->m_position, direction, sightComponent.lock()->m_angle);
                    if (isInFoV) {
                        seenEntities.emplace_back(overlapEntity);
                        continue;
                    }

                    glm::vec2 leftDirection = glm::rotate(direction, glm::radians(sightComponent.lock()->m_angle / 2.0f));
                    glm::vec2 leftPosition = transformComponent.lock()->m_position + leftDirection;
                    const bool isInLeftLoS = IsInLoS(physicsComponent, transformComponent.lock()->m_position, leftPosition, sightComponent.lock()->m_distance, overlapEntity);
                    if (isInLeftLoS) {
                        seenEntities.emplace_back(overlapEntity);
                        continue;
                    }

                    glm::vec2 rightDirection = glm::rotate(direction, glm::radians(-sightComponent.lock()->m_angle / 2.0f));
                    glm::vec2 rightPosition = transformComponent.lock()->m_position + rightDirection;
                    const bool isInRightLoS = IsInLoS(physicsComponent, transformComponent.lock()->m_position, rightPosition, sightComponent.lock()->m_distance, overlapEntity);
                    if (isInRightLoS) {
                        seenEntities.emplace_back(overlapEntity);
                        continue;
                    }
                }

                std::weak_ptr<ColliderComponent> overlapColliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(overlapEntity);
                glm::vec2 overlapCenter = overlapTransformComponent.lock()->m_position;
                glm::vec2 overlapExtents = overlapColliderComponent.lock()->m_size / 2.0f;
                std::vector<Entity> overlapOverlapEntities;
                const bool isOverlapOverlap = physicsComponent.lock()->Overlap(overlapCenter, overlapExtents, overlapOverlapEntities);
                if (isOverlapOverlap) {
                    for (const auto& overlapOverlapEntity : overlapOverlapEntities) {
                        if (overlapOverlapEntity == entity) {
                            seenEntities.emplace_back(overlapEntity);
                            break;
                        }
                    }
                }
            }
        }

        for (std::vector<Entity>::const_iterator it = sightComponent.lock()->m_seenEntities.begin(); it != sightComponent.lock()->m_seenEntities.end();) {
            Entity seenEntity = *it;
            std::vector<Entity>::const_iterator foundEntity = std::find(seenEntities.begin(), seenEntities.end(), seenEntity);
            if (foundEntity == seenEntities.end()) {
                Event newSightEvent;
                newSightEvent.eventType = EventType::SEEN_LOST_ENTITY;
                newSightEvent.sight.playerID = playerID;
                newSightEvent.sight.entity = seenEntity;
                PushEvent(newSightEvent);

                it = sightComponent.lock()->m_seenEntities.erase(it);
            } else {
                ++it;
            }
        }

        for (std::vector<Entity>::const_iterator it = seenEntities.begin(); it != seenEntities.end(); ++it) {
            Entity seenEntity = *it;
            const bool isSeen = sightComponent.lock()->IsSeen(seenEntity);
            if (!isSeen) {
                Event newSightEvent;
                newSightEvent.eventType = EventType::SEEN_NEW_ENTITY;
                newSightEvent.sight.playerID = playerID;
                newSightEvent.sight.entity = seenEntity;
                PushEvent(newSightEvent);

                sightComponent.lock()->m_seenEntities.emplace_back(seenEntity);
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::Render()
{
    OPTICK_EVENT();

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameServer->GetComponentManager().GetComponent<SightComponent>(entity);
        Draw(transformComponent, sightComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::IsInFoV(glm::vec2 position, glm::vec2 targetPosition, glm::vec2 direction, F32 angle) const
{
    glm::vec2 vectorToTarget = targetPosition - position;
    glm::vec2 directionToTarget = glm::normalize(vectorToTarget);
    F32 angleToTarget = glm::degrees(glm::angle(direction, directionToTarget));
    return angleToTarget <= angle * 0.5f;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemServer::IsInLoS(std::weak_ptr<PhysicsComponent> physicsComponent, glm::vec2 position, glm::vec2 targetPosition, F32 distance, Entity entity) const
{
    glm::vec2 vectorToTarget = targetPosition - position;
    glm::vec2 directionToTarget = glm::normalize(vectorToTarget);
    glm::vec2 distanceToTarget = position + directionToTarget * distance;

    PhysicsComponent::RaycastHit raycastHit;
    if (physicsComponent.lock()->Raycast(position, distanceToTarget, raycastHit)) {
        return raycastHit.m_entity == entity;
    }

    return false;
}
}
