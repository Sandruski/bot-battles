#include "SightSystemClient.h"

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
    //m_signature |= 1 << static_cast<U16>(ComponentType::BOT); // TODO: since all remote players are bots, do we really need this?
    m_signature |= 1 << static_cast<U16>(ComponentType::REMOTE_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::Update()
{
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
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    //rendererComponent.SetWireframe(true);

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !sightComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(transformComponent.lock()->m_rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        glm::vec3 scale = glm::vec3(sightComponent.lock()->m_distance, sightComponent.lock()->m_distance, 0.0f);
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);

        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
        U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        U32 colorLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(Green));

        U32 pctLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "pct");
        glUniform1f(pctLoc, 1.0f);

        glBindVertexArray(rendererComponent.m_meshResource.lock()->GetVAO());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    //rendererComponent.SetWireframe(false);

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
    return physicsComponent.Raycast(position, distanceToTarget, raycastHit);
}
}
