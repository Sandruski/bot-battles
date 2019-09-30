#include "ModuleRenderer.h"

#include "Engine.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"
#include "DebugDrawer.h"

#include "Log.h"
#include "Colors.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleRenderer::ModuleRenderer() : 
		m_renderer(nullptr), 
		m_backgroundColor(Black),
		m_isInitOk(false),
		m_isDebugDraw(true)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleRenderer::~ModuleRenderer()
	{
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleRenderer::GetName() const
	{
		return "Renderer";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleRenderer::StartUp()
	{
		m_renderer = SDL_CreateRenderer(g_engine->GetWindow().GetWindow(), -1, SDL_RENDERER_ACCELERATED);
		if (m_renderer == nullptr)
		{
			LOG("Renderer could not be created! SDL Error: %s", SDL_GetError());
			return m_isInitOk;
		}

		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleRenderer::ShutDown()
	{
		if (m_isInitOk)
		{
			SDL_DestroyRenderer(m_renderer);
			m_renderer = nullptr;
		}

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleRenderer::Draw()
	{
		if (m_isInitOk)
		{
			SDL_SetRenderDrawColor(m_renderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
			SDL_RenderClear(m_renderer);

			/*
			1. All level geometry
			2. All debug geometry
			3. The editor
			4. Swap buffers
			*/

			// TODO: for auto (iterate all game objects)
			//SDL_RenderCopy(m_renderer, gTexture, nullptr, nullptr);
			SDL_RenderCopy(m_renderer, g_engine->m_resourceTexture->GetTexture(), nullptr, nullptr);

			if (m_isDebugDraw)
			{
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
			}

			SDL_RenderPresent(m_renderer);
		}

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Renderer* ModuleRenderer::GetRenderer() const
	{
		return m_renderer;
	}
}
