#include "CollisionSystem.h"

#include "ColliderComponent.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void CollisionSystem::DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<ColliderComponent> colliderComponent, const glm::vec4& color) const
{
    glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
    F32 rotation = transformComponent.lock()->m_rotation;
    glm::vec3 scale = glm::vec3(colliderComponent.lock()->m_size.x, colliderComponent.lock()->m_size.y, 1.0f);

    switch (colliderComponent.lock()->m_shapeType) {

    case ColliderComponent::ShapeType::CIRCLE: {
        rendererComponent.DrawCircle(position, rotation, scale, 36, 360.0f, 0.5f, color, false);
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
}
