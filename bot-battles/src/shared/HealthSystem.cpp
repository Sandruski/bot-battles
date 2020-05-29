#include "HealthSystem.h"

#include "HealthComponent.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void HealthSystem::Draw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<HealthComponent> healthComponent, const glm::vec4& color) const
{
    glm::vec3 position = glm::vec3(transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y, static_cast<F32>(LayerType::LIFEBAR));
    F32 rotation = transformComponent.lock()->m_rotation;
    glm::vec3 scale = glm::vec3(50.0f, 50.0f, 0.0f);
    rendererComponent.DrawQuad(position, rotation, scale, color, true);
}
}
