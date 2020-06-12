#include "MovementSystem.h"

#include "Game.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void MovementSystem::DebugDraw(std::weak_ptr<TransformComponent> transformComponent) const
{
    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    glm::vec2 fromPosition = transformComponent.lock()->m_position;
    glm::vec2 direction = transformComponent.lock()->GetDirection();
    glm::vec2 toPosition = fromPosition + direction * 40.0f;
    glm::vec4 color = Green;
    color.a = 0.5f;
    rendererComponent.lock()->DrawLine(fromPosition, toPosition, static_cast<F32>(transformComponent.lock()->m_layerType), color);
}
}
