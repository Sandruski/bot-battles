#include "WindowSystem.h"

#include "Game.h"

#include "Defines.h"

#include "SingletonWindowComponent.h"


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
			ELOG("SDL video subsystem could not be initialized! SDL Error: %s", SDL_GetError());
			return false;
		}

		std::shared_ptr<SingletonWindowComponent> window = g_game->GetSingletonWindowComponent();

		window->m_window = SDL_CreateWindow(g_game->GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->m_width, window->m_height, SDL_WINDOW_SHOWN);
		if (window->m_window == nullptr)
		{
			ELOG("Window could not be created! SDL Error: %s", SDL_GetError());
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

		ILOG("Quitting SDL video subsystem");
		SDL_QuitSubSystem(SDL_INIT_VIDEO);

		return true;
	}
}
