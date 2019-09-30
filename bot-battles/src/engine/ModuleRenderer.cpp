#include "ModuleRenderer.h"

#include "Engine.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"

#include "Log.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleRenderer::ModuleRenderer() : m_renderer(nullptr), m_isInitOk(false)
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
			SDL_Quit();
			return m_isInitOk;
		}

		SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
			SDL_RenderClear(m_renderer);

			// TODO: for auto (iterate all game objects)
			//SDL_RenderCopy(m_renderer, gTexture, nullptr, nullptr);
			SDL_RenderCopy(m_renderer, g_engine->m_resourceTexture->GetTexture(), nullptr, nullptr);

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
