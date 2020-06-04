#include "SightSystem.h"

#include "Game.h"
#include "RendererComponent.h"
#include "SightComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void SightSystem::Draw(std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<SightComponent> sightComponent) const
{
    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    glm::vec3 position = glm::vec3(transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y, static_cast<F32>(LayerType::SIGHT));
    F32 rotation = transformComponent.lock()->m_rotation + sightComponent.lock()->m_angle / 2.0f;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec4 color = White;
    color.a = 0.5f;
    rendererComponent.lock()->DrawCircle(position, rotation, scale, 16, sightComponent.lock()->m_angle, sightComponent.lock()->m_distance, color, true);
}
}
