#include "RendererSystem.h"

#include "DebugDrawer.h"
#include "Game.h"
#include "ResourceTexture.h"
#include "ComponentManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"

#include "Colors.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
	m_signature.set(static_cast<std::size_t>(ComponentType::SPRITE));
	m_signature.set(static_cast<std::size_t>(ComponentType::TRANSFORM));
}

//----------------------------------------------------------------------------------------------------
RendererSystem::~RendererSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::StartUp()
{
	std::shared_ptr<SingletonRendererComponent> renderer = g_game->GetSingletonRendererComponent();
	std::shared_ptr<SingletonWindowComponent> window = g_game->GetSingletonWindowComponent();

    renderer->m_renderer = SDL_CreateRenderer(window->m_window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer->m_renderer == nullptr) {
        LOG("Renderer could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
	std::shared_ptr<SingletonRendererComponent> renderer = g_game->GetSingletonRendererComponent();

    BeginDraw(*renderer);

    /*
		1. All level geometry
		2. All debug geometry -> Maybe from each state/system? Render()
		3. The editor -> Maybe from each system? Render()
		4. Swap buffers
		*/

    for (auto& entity : m_entities) {

		std::shared_ptr<SpriteComponent> sprite = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
		std::shared_ptr<TransformComponent> transform = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);

		SDL_Rect renderQuad = { 
			static_cast<I32>(transform->m_position.x), 
			static_cast<I32>(transform->m_position.y), 
			sprite->m_texture->GetWidth(),
			sprite->m_texture->GetHeight() 
		};

		SDL_RenderCopy(renderer->m_renderer, sprite->m_texture->GetTexture(), nullptr, &renderQuad);
    }

    if (renderer->m_isDebugDraw) {
        /*
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

    EndDraw(*renderer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::ShutDown()
{
    std::shared_ptr<SingletonRendererComponent> renderer = g_game->GetSingletonRendererComponent();

    SDL_DestroyRenderer(renderer->m_renderer);
    renderer->m_renderer = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::BeginDraw(const SingletonRendererComponent& renderer) const
{
    SDL_SetRenderDrawColor(renderer.m_renderer, renderer.m_backgroundColor.r, renderer.m_backgroundColor.g, renderer.m_backgroundColor.b, renderer.m_backgroundColor.a);
    SDL_RenderClear(renderer.m_renderer);
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::EndDraw(const SingletonRendererComponent& renderer) const
{
    SDL_RenderPresent(renderer.m_renderer);
}
}
