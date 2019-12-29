#include "RendererSystem.h"

#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "FontResource.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"

#include "RendererComponent.h"
#include "WindowComponent.h"

#include "Colors.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE); // TODO: debug draw should not need having a sprite component!
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::StartUp()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    U32 flags = SDL_RENDERER_ACCELERATED;
    if (rendererComponent.m_isVsync) {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    rendererComponent.m_renderer = SDL_CreateRenderer(windowComponent.m_window, -1, flags);
    if (rendererComponent.m_renderer == nullptr) {
        ELOG("Renderer could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PreRender()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    SDL_SetRenderDrawColor(rendererComponent.m_renderer, rendererComponent.m_backgroundColor.r, rendererComponent.m_backgroundColor.g, rendererComponent.m_backgroundColor.b, rendererComponent.m_backgroundColor.a);
    SDL_RenderClear(rendererComponent.m_renderer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

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

        if (!spriteComponent.lock()->m_sprite.expired()) {
            SDL_Rect renderQuad = {
                static_cast<I32>(transformComponent.lock()->m_position.x),
                static_cast<I32>(transformComponent.lock()->m_position.y),
                static_cast<I32>(spriteComponent.lock()->m_sprite.lock()->GetWidth()),
                static_cast<I32>(spriteComponent.lock()->m_sprite.lock()->GetHeight())
            };

            SDL_RenderCopy(rendererComponent.m_renderer, spriteComponent.lock()->m_sprite.lock()->GetTexture(), nullptr, &renderQuad);
        }

        if (rendererComponent.m_isDebugDraw) {
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
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    SDL_RenderPresent(rendererComponent.m_renderer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::ShutDown()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    SDL_DestroyRenderer(rendererComponent.m_renderer);
    rendererComponent.m_renderer = nullptr;

    return true;
}
}
