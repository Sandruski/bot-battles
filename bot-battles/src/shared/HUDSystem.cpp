#include "HUDSystem.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "TextResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HUDSystem::HUDSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::TEXT);
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::StartUp()
{
    m_fps = g_game->GetEntityManager().AddEntity();
    g_game->GetComponentManager().AddComponent<TransformComponent>(m_fps);
    std::weak_ptr<TextResource> textResource = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    textResource.lock()->SetFont(rendererComponent.m_font);
    textResource.lock()->SetColor(Green);
    std::weak_ptr<TextComponent> textComponent = g_game->GetComponentManager().AddComponent<TextComponent>(m_fps);
    textComponent.lock()->m_text = textResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::PreUpdate()
{
    std::weak_ptr<TextComponent> textComponent = g_game->GetComponentManager().GetComponent<TextComponent>(m_fps);
    std::string fps = std::to_string(static_cast<U32>(Time::GetInstance().GetFps()));
    textComponent.lock()->m_text.lock()->SetText(fps.c_str());
    textComponent.lock()->m_text.lock()->ReLoad();

    std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(m_fps);
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    transformComponent.lock()->m_position.x = static_cast<F32>(windowComponent.m_width - textComponent.lock()->m_text.lock()->GetWidth());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::Render()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<TextComponent> textComponent = g_game->GetComponentManager().GetComponent<TextComponent>(entity);

        if (!textComponent.lock()->m_text.expired()) {
            SDL_Rect renderQuad = {
                static_cast<I32>(transformComponent.lock()->m_position.x),
                static_cast<I32>(transformComponent.lock()->m_position.y),
                static_cast<I32>(textComponent.lock()->m_text.lock()->GetWidth()),
                static_cast<I32>(textComponent.lock()->m_text.lock()->GetHeight())
            };

            SDL_RenderCopy(rendererComponent.m_renderer, textComponent.lock()->m_text.lock()->GetTexture(), nullptr, &renderQuad);
        }

        if (rendererComponent.m_isDebugDraw) {
            // TODO
        }
    }

    return true;
}
}
