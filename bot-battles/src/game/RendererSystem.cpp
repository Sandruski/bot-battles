#include "RendererSystem.h"

#include "DebugDrawer.h"
#include "Game.h"
#include "ResourceTexture.h"

#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"

#include "Colors.h"
#include "Log.h"

#include <SDL.h>

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
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
		2. All debug geometry
		3. The editor
		4. Swap buffers
		*/

    for (auto& entity : m_entities) {
        //SDL_RenderCopy(m_renderer, gTexture, nullptr, nullptr);
        //SDL_RenderCopy(m_renderer, g_engine->GetGame().m_resourceTexture->GetTexture(), nullptr, nullptr);
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
