#include "HUDSystem.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HUDSystem::HUDSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::TEXT);
}

//----------------------------------------------------------------------------------------------------
HUDSystem::~HUDSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::StartUp()
{
    m_fps = g_game->GetEntityManager().AddEntity();
    g_game->GetComponentManager().AddComponent<TransformComponent>(m_fps);
    std::shared_ptr<TextResource> textResource = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();
    textResource->SetFont(singletonRenderer->m_font);
    textResource->SetColor(Green);
    std::shared_ptr<TextComponent> textComponent = g_game->GetComponentManager().AddComponent<TextComponent>(m_fps);
    textComponent->m_text = textResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::PreUpdate()
{
    std::shared_ptr<TextComponent> textComponent = g_game->GetComponentManager().GetComponent<TextComponent>(m_fps);
    std::string fps = std::to_string(static_cast<U32>(Time::GetInstance().GetFps()));
    textComponent->m_text->SetText(fps.c_str());
    textComponent->m_text->ReLoad();

    std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(m_fps);
    std::shared_ptr<SingletonWindowComponent> singletonWindow = g_game->GetSingletonWindowComponent();
    transformComponent->m_position.x = static_cast<F32>(singletonWindow->m_width - textComponent->m_text->GetWidth());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::Render()
{
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();

    for (auto& entity : m_entities) {

        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::shared_ptr<TextComponent> textComponent = g_game->GetComponentManager().GetComponent<TextComponent>(entity);

        if (textComponent->m_text != nullptr) {
            SDL_Rect renderQuad = {
                static_cast<I32>(transformComponent->m_position.x),
                static_cast<I32>(transformComponent->m_position.y),
                static_cast<I32>(textComponent->m_text->GetWidth()),
                static_cast<I32>(textComponent->m_text->GetHeight())
            };

            SDL_RenderCopy(singletonRenderer->m_renderer, textComponent->m_text->GetTexture(), nullptr, &renderQuad);
        }

        if (singletonRenderer->m_isDebugDraw) {
            // TODO
        }
    }

    return true;
}
}
