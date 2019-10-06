#include "RendererSystem.h"

#include "Game.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"
#include "DebugDrawer.h"

#include "ComponentManager.h"
#include "RendererComponent.h"

#include "Log.h"
#include "Colors.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	RendererSystem::RendererSystem()
	{
		m_signature.set(ComponentType::TRANSFORM);
		m_signature.set(ComponentType::SPRITE);
	}

	//----------------------------------------------------------------------------------------------------
	RendererSystem::~RendererSystem()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool RendererSystem::StartUp()
	{
		RendererComponent& renderer = g_game->GetComponentManager().GetSingletonComponent<RendererComponent>();
		
		renderer.m_renderer = SDL_CreateRenderer(g_game->GetWindow().GetWindow(), -1, SDL_RENDERER_ACCELERATED);
		if (renderer.m_renderer == nullptr)
		{
			LOG("Renderer could not be created! SDL Error: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool RendererSystem::ShutDown()
	{
		RendererComponent& renderer = g_game->GetComponentManager().GetSingletonComponent<RendererComponent>();

		SDL_DestroyRenderer(renderer.m_renderer);
		renderer.m_renderer = nullptr;

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool RendererSystem::Render()
	{
		RendererComponent& renderer = g_game->GetComponentManager().GetSingletonComponent<RendererComponent>();

		BeginDraw(renderer);

		/*
		1. All level geometry
		2. All debug geometry
		3. The editor
		4. Swap buffers
		*/

		for (auto& entity : m_entities)
		{
			//SDL_RenderCopy(m_renderer, gTexture, nullptr, nullptr);
			//SDL_RenderCopy(m_renderer, g_engine->GetGame().m_resourceTexture->GetTexture(), nullptr, nullptr);
		}

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
	void RendererSystem::BeginDraw(const RendererComponent& renderer) const
	{
		SDL_SetRenderDrawColor(renderer.m_renderer, renderer.m_backgroundColor.r, renderer.m_backgroundColor.g, renderer.m_backgroundColor.b, renderer.m_backgroundColor.a);
		SDL_RenderClear(renderer.m_renderer);
	}

	//----------------------------------------------------------------------------------------------------
	void RendererSystem::EndDraw(const RendererComponent& renderer) const
	{
		SDL_RenderPresent(renderer.m_renderer);
	}
}
