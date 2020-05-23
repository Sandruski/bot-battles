#include "SightSystem.h"

#include "RendererComponent.h"
#include "SightComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void SightSystem::DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<SightComponent> sightComponent) const
{
    glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
    F32 rotation = transformComponent.lock()->m_rotation + sightComponent.lock()->m_angle / 2.0f;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec4 color = Red;
    color.a = 0.5f;
    rendererComponent.DrawCircle(position, rotation, scale, 16, sightComponent.lock()->m_angle, sightComponent.lock()->m_distance, color, true);

    /*
for (const auto& seenEntity : sightComponent.lock()->m_seenEntities) {
    if (seenEntity == entity) {
        continue;
    }

    std::weak_ptr<TransformComponent> seenTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(seenEntity);
    std::weak_ptr<ColliderComponent> seenColliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(seenEntity);
    if (!seenTransformComponent.lock()->m_isEnabled) {
        continue;
    }

    glm::vec3 seenPosition = seenTransformComponent.lock()->GetDebugPositionAndLayer();
    F32 seenRotation = seenTransformComponent.lock()->m_rotation;
    glm::vec3 seenScale = glm::vec3(seenColliderComponent.lock()->m_size.x * 1.5f, seenColliderComponent.lock()->m_size.y * 2.0f, 0.0f);
    glm::vec4 seenColor = Blue;
    seenColor.a = 0.5f;
    rendererComponent.DrawCircle(seenPosition, seenRotation, seenScale, 36, 360.0f, 0.5f, seenColor, true);
}
*/
}
}
