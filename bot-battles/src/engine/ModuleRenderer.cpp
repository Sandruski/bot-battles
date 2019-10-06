#include "ModuleRenderer.h"

#include "Game.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"
#include "DebugDrawer.h"

#include "ComponentManager.h"
#include "ComponentRenderer.h"

#include "Log.h"
#include "Colors.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	const char* ModuleRenderer::GetName()
	{
		return "Renderer";
	}

	//----------------------------------------------------------------------------------------------------
	ModuleRenderer::ModuleRenderer() : Module(true)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleRenderer::~ModuleRenderer()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleRenderer::StartUp()
	{
		ComponentRenderer& renderer = g_game->GetComponentManager().GetSingletonComponent<ComponentRenderer>();
		
		renderer.m_renderer = SDL_CreateRenderer(g_game->GetWindow().GetWindow(), -1, SDL_RENDERER_ACCELERATED);
		if (renderer.m_renderer == nullptr)
		{
			LOG("Renderer could not be created! SDL Error: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleRenderer::ShutDown()
	{
		ComponentRenderer& renderer = g_game->GetComponentManager().GetSingletonComponent<ComponentRenderer>();

		SDL_DestroyRenderer(renderer.m_renderer);
		renderer.m_renderer = nullptr;

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleRenderer::Draw() const
	{
		ComponentRenderer& renderer = g_game->GetComponentManager().GetSingletonComponent<ComponentRenderer>();

		BeginDraw(renderer);

		/*
		1. All level geometry
		2. All debug geometry
		3. The editor
		4. Swap buffers
		*/

		// TODO: for auto (iterate all game objects)
		//SDL_RenderCopy(m_renderer, gTexture, nullptr, nullptr);
		//SDL_RenderCopy(m_renderer, g_engine->GetGame().m_resourceTexture->GetTexture(), nullptr, nullptr);

		if (renderer.m_isDebugDraw)
		{
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

		EndDraw(renderer);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleRenderer::BeginDraw(const ComponentRenderer& renderer) const
	{
		SDL_SetRenderDrawColor(renderer.m_renderer, renderer.m_backgroundColor.r, renderer.m_backgroundColor.g, renderer.m_backgroundColor.b, renderer.m_backgroundColor.a);
		SDL_RenderClear(renderer.m_renderer);
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleRenderer::EndDraw(const ComponentRenderer& renderer) const
	{
		SDL_RenderPresent(renderer.m_renderer);
	}
}
