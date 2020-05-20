#include "CollisionSystemServer.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystemServer::CollisionSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::DebugRender()
{
    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !colliderComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::vec3 scale = glm::vec3(colliderComponent.lock()->m_size.x, colliderComponent.lock()->m_size.y, 0.0f);
        glm::vec4 color = Green;
        color.a = 0.5f;

        switch (colliderComponent.lock()->m_shapeType) {

        case ColliderComponent::ShapeType::CIRCLE: {
            rendererComponent.DrawCircle(position, rotation, scale, 360.0f, 0.5f, color, false);
            break;
        }

        case ColliderComponent::ShapeType::BOX: {
            rendererComponent.DrawQuad(position, rotation, scale, color, false);
            break;
        }

        default: {
            break;
        }
        }
    }

    return true;
}
}
