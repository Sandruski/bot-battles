#include "RendererSystem.h"

#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "FontResource.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"

#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"

#include "Colors.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE); // TODO: debug draw should not need having a sprite component!
}

//----------------------------------------------------------------------------------------------------
RendererSystem::~RendererSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::StartUp()
{
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();
    std::shared_ptr<SingletonWindowComponent> singletonWindow = g_game->GetSingletonWindowComponent();

    singletonRenderer->m_renderer = SDL_CreateRenderer(singletonWindow->m_window, -1, SDL_RENDERER_ACCELERATED);
    if (singletonRenderer->m_renderer == nullptr) {
        ELOG("Renderer could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    std::shared_ptr<FontResource> font = g_game->GetResourceManager().AddResource<FontResource>("Dosis-Regular.ttf", FONTS_DIR, true);
    font->SetSize(30);
    singletonRenderer->m_font = font;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PreRender()
{
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();

    SDL_SetRenderDrawColor(singletonRenderer->m_renderer, singletonRenderer->m_backgroundColor.r, singletonRenderer->m_backgroundColor.g, singletonRenderer->m_backgroundColor.b, singletonRenderer->m_backgroundColor.a);
    SDL_RenderClear(singletonRenderer->m_renderer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();

    /*
		1. All level geometry
		2. All debug geometry -> Maybe from each state/system? Render()
		3. The editor -> Maybe from each system? Render()
		4. Swap buffers
	*/

    std::sort(m_entities.begin(), m_entities.end(), [](Entity entity1, Entity entity2) {
        std::weak_ptr<TransformComponent> transformComponent1 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity1);
        std::weak_ptr<TransformComponent> transformComponent2 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity2);
        return transformComponent1.lock()->m_position.z < transformComponent2.lock()->m_position.z;
    });

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);

        if (spriteComponent.lock()->m_sprite != nullptr) {
            SDL_Rect renderQuad = {
                static_cast<I32>(transformComponent.lock()->m_position.x),
                static_cast<I32>(transformComponent.lock()->m_position.y),
                static_cast<I32>(spriteComponent.lock()->m_sprite->GetWidth()),
                static_cast<I32>(spriteComponent.lock()->m_sprite->GetHeight())
            };

            SDL_RenderCopy(singletonRenderer->m_renderer, spriteComponent.lock()->m_sprite->GetTexture(), nullptr, &renderQuad);
        }

        if (singletonRenderer->m_isDebugDraw) {
            /*
            DebugDrawer::DrawQuad(
                {
                    (int)transform->m_position.x,
                    (int)transform->m_position.y,
                    (int)transform->m_position.x + 10,
                    (int)transform->m_position.y + 10,
                },
                Red);

			DebugDrawer::DrawQuad(
				{
				(int)g_engine->GetWindow().GetWidth() / 4,
				(int)g_engine->GetWindow().GetHeight() / 4,
				(int)g_engine->GetWindow().GetWidth() / 2,
				(int)g_engine->GetWindow().GetHeight() / 2,
				},
				Red);

			DebugDrawer::DrawCircle(
				(int)g_engine->GetWindow().GetWidth() / 4,
				(int)g_engine->GetWindow().GetHeight() / 4,
				50,
				Green);

			DebugDrawer::DrawLine(
				{
				0,
				(int)g_engine->GetWindow().GetHeight() / 2,
				(int)g_engine->GetWindow().GetWidth(),
				(int)g_engine->GetWindow().GetHeight() / 2,
				},
				Blue);
			*/
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PostRender()
{
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();

    SDL_RenderPresent(singletonRenderer->m_renderer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::ShutDown()
{
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();

    SDL_DestroyRenderer(singletonRenderer->m_renderer);
    singletonRenderer->m_renderer = nullptr;

    return true;
}
}
