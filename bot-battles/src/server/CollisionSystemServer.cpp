#include "CollisionSystemServer.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ShaderResource.h"
#include "SpriteComponent.h"
#include "State.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystemServer::CollisionSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        colliderComponent.lock()->m_position = transformComponent.lock()->m_position;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::DebugRender()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();
    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    rendererComponent.SetWireframe(true);

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !colliderComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 position = transformComponent.lock()->GetDebugPosition();
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(transformComponent.lock()->m_rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        glm::vec3 scale = glm::vec3(colliderComponent.lock()->m_size.x, colliderComponent.lock()->m_size.y, 0.0f);
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

    rendererComponent.SetWireframe(false);

    return true;
}
}
