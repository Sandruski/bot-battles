#include "CollisionSystemClient.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ShaderResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystemClient::CollisionSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemClient::Update()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::PLAY) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);

        colliderComponent.lock()->m_position.x = transformComponent.lock()->m_position.x;
        colliderComponent.lock()->m_position.y = transformComponent.lock()->m_position.y;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemClient::DebugRender()
{
    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();

    rendererComponent.SetWireframe(true);

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !colliderComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transformComponent.lock()->m_position);
        model = glm::rotate(model, glm::radians(transformComponent.lock()->m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(colliderComponent.lock()->m_size.x, colliderComponent.lock()->m_size.y, 0.0f));

        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_resolution.x), static_cast<F32>(windowComponent.m_resolution.y), 0.0f, -1.0f, 1.0f);
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
