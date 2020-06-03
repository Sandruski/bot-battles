#include "HealthSystem.h"

#include "Game.h"
#include "HealthComponent.h"
#include "RendererComponent.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void HealthSystem::Draw(PlayerID playerID, std::weak_ptr<HealthComponent> healthComponent) const
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    WindowComponent& windowComponent = g_game->GetWindowComponent();

    F32 rotation = 0.0f;

    glm::vec3 backgroundScale = glm::vec3(static_cast<F32>(windowComponent.m_baseResolution.x) / 3.0f, 20.0f, 0.0f);
    glm::vec3 backgroundPosition = glm::vec3(0.0f, 20.0f, static_cast<F32>(LayerType::LIFEBAR));
    switch (playerID) {
    case 0: {
        backgroundPosition.x = static_cast<F32>(windowComponent.m_baseResolution.x) / 4.0f;
        break;
    }
    case 1: {
        backgroundPosition.x = static_cast<F32>(windowComponent.m_baseResolution.x) - static_cast<F32>(windowComponent.m_baseResolution.x) / 4.0f;
        break;
    }
    default: {
        break;
    }
    }
    glm::vec4 backgroundColor = Black;
    backgroundColor.a = 0.5f;
    rendererComponent.DrawQuad(backgroundPosition, rotation, backgroundScale, backgroundColor, true);

    glm::vec3 scale = backgroundScale;
    scale.x *= static_cast<F32>(healthComponent.lock()->m_currentHP) / static_cast<F32>(healthComponent.lock()->m_maxHP);
    glm::vec3 position = backgroundPosition;
    position.x -= (backgroundScale.x - scale.x) / 2.0f;
    glm::vec4 color = White;
    switch (playerID) {
    case 0: {
        color = Red;
        break;
    }
    case 1: {
        color = Blue;
        break;
    }
    default: {
        break;
    }
    }
    rendererComponent.DrawQuad(position, rotation, scale, color, true);
}
}
