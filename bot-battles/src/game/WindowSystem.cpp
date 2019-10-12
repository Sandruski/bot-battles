#include "WindowSystem.h"

#include "Game.h"

#include "Log.h"
#include "Memory.h"
#include "Defines.h"

#include "SingletonWindowComponent.h"

#include <SDL.h>
#include <stdio.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	WindowSystem::WindowSystem()
	{
	}

	//----------------------------------------------------------------------------------------------------
	WindowSystem::~WindowSystem()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool WindowSystem::StartUp()
	{
		if (SDL_Init(SDL_INIT_VIDEO) == SDL_ERROR)
		{
			LOG("SDL video subsystem could not be initialized! SDL Error: %s", SDL_GetError());
			return false;
		}

		std::shared_ptr<SingletonWindowComponent> window = g_game->GetSingletonWindowComponent();

		window->m_window = SDL_CreateWindow(g_game->GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->m_width, window->m_height, SDL_WINDOW_SHOWN);
		if (window->m_window == nullptr)
		{
			LOG("Window could not be created! SDL Error: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool WindowSystem::ShutDown()
	{
		std::shared_ptr<SingletonWindowComponent> window = g_game->GetSingletonWindowComponent();

		SDL_DestroyWindow(window->m_window);
		window->m_window = nullptr;

		LOG("Quitting SDL video subsystem");
		SDL_QuitSubSystem(SDL_INIT_VIDEO);

		return true;
	}
}
