#include "CollisionSystemServer.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "RigidbodyComponent.h"
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
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::DebugRender()
{
    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();
    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    rendererComponent.SetWireframe(true);

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !colliderComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
        b2Vec2 pos = rigidbodyComponent.lock()->m_body->GetPosition();
        position.x = METERS_TO_PIXELS(pos.x);
        position.y = METERS_TO_PIXELS(pos.y);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(transformComponent.lock()->m_rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        glm::vec3 scale = glm::vec3(colliderComponent.lock()->m_size.x, colliderComponent.lock()->m_size.y, 0.0f);
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);

        std::vector<MeshResource::Vertex> vertices = MeshResource::GetQuadVertices();
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
