#include "CollisionSystem.h"

#include "ColliderComponent.h"
#include "Game.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void CollisionSystem::DebugDraw(std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<ColliderComponent> colliderComponent) const
{
    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
    F32 rotation = transformComponent.lock()->m_rotation;
    glm::vec3 scale = glm::vec3(colliderComponent.lock()->m_size.x, colliderComponent.lock()->m_size.y, 0.0f);
    glm::vec4 color = Green;
    color.a = 0.5f;
    switch (colliderComponent.lock()->m_shapeType) {
    case ColliderComponent::ShapeType::CIRCLE: {
        rendererComponent.lock()->DrawCircle(position, rotation, scale, 36, 360.0f, 0.5f, color, false);
        break;
    }
    case ColliderComponent::ShapeType::BOX: {
        rendererComponent.lock()->DrawQuad(position, rotation, scale, color, false);
        break;
    }
    default: {
        break;
    }
    }
}
}
