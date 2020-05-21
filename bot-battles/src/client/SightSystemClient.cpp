#include "SightSystemClient.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "RigidbodyComponent.h"
#include "ShaderResource.h"
#include "SightComponent.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightSystemClient::SightSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::BOT); // TODO: since all remote players are bots, do we really need this?
    m_signature |= 1 << static_cast<U16>(ComponentType::REMOTE_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.m_entity >= INVALID_ENTITY) {
        return true;
    }

    if (g_gameClient->GetLinkingContext().GetNetworkID(clientComponent.m_entity) >= INVALID_NETWORK_ID) {
        return true;
    }

    std::weak_ptr<TransformComponent> localTransformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(clientComponent.m_entity);
    std::weak_ptr<SightComponent> localSightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(clientComponent.m_entity);
    if (!localTransformComponent.lock()->m_isEnabled || !localSightComponent.lock()->m_isEnabled) {
        return true;
    }

    glm::vec2 direction = localTransformComponent.lock()->GetDirection();

    PhysicsComponent& physicsComponent = g_gameClient->GetPhysicsComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> remoteTransformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        if (!remoteTransformComponent.lock()->m_isEnabled) {
            continue;
        }

        const bool isInFoV = IsInFoV(localTransformComponent.lock()->m_position, remoteTransformComponent.lock()->m_position, direction, localSightComponent.lock()->m_angle);
        if (!isInFoV && localSightComponent.lock()->m_target < INVALID_ENTITY) {
            localSightComponent.lock()->m_target = INVALID_ENTITY;
            Event newSightEvent;
            newSightEvent.eventType = EventType::SEEN_BOT_ENTER;
            continue;
        }

        const bool isInLoS = IsInLoS(physicsComponent, localTransformComponent.lock()->m_position, remoteTransformComponent.lock()->m_position, localSightComponent.lock()->m_distance);
        if (!isInLoS && localSightComponent.lock()->m_target < INVALID_ENTITY) {
            localSightComponent.lock()->m_target = INVALID_ENTITY;
            Event newSightEvent;
            newSightEvent.eventType = EventType::SEEN_BOT_ENTER;
            continue;
        }

        if (localSightComponent.lock()->m_target >= INVALID_ENTITY) {
            localSightComponent.lock()->m_target = entity;
            Event newSightEvent;
            newSightEvent.eventType = EventType::SEEN_BOT_EXIT;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::DebugRender()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
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
bool SightSystemClient::IsInFoV(glm::vec2 position, glm::vec2 targetPosition, glm::vec2 direction, F32 angle) const
{
    glm::vec2 vectorToTarget = targetPosition - position;
    glm::vec2 directionToTarget = glm::normalize(vectorToTarget);
    F32 angleToTarget = glm::angle(direction, directionToTarget);
    return angleToTarget <= angle;
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::IsInLoS(PhysicsComponent& physicsComponent, glm::vec2 position, glm::vec2 targetPosition, F32 distance) const
{
    glm::vec2 vectorToTarget = targetPosition - position;
    glm::vec2 directionToTarget = glm::normalize(vectorToTarget);
    glm::vec2 distanceToTarget = directionToTarget * distance;

    PhysicsComponent::RaycastHit raycastHit;
    if (physicsComponent.Raycast(position, distanceToTarget, raycastHit)) {
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(raycastHit.m_entity);
        if (!botComponent.expired()) {
            return true;
        }
    }

    return false;
}
}
