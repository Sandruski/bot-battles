#include "MovementSystem.h"

#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void MovementSystem::DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, const glm::vec4& color) const
{
    glm::vec3 fromPosition = transformComponent.lock()->GetDebugPositionAndLayer();
    glm::vec2 direction = transformComponent.lock()->GetDirection();
    glm::vec3 toPosition = fromPosition + glm::vec3(direction.x, direction.y, 0.0f) * 40.0f;
    rendererComponent.DrawLine(fromPosition, toPosition, color);
}
}
