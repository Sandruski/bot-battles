#include "ModuleWindow.h"

#include "Game.h"

#include "Log.h"
#include "Memory.h"
#include "Defines.h"

#include <SDL.h>
#include <stdio.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	const char* ModuleWindow::GetName()
	{
		return "Window";
	}

	//----------------------------------------------------------------------------------------------------
	ModuleWindow::ModuleWindow() : Module(true),
		m_window(nullptr),
		m_width(640),
		m_height(480),
		m_isInitOk(false)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleWindow::~ModuleWindow() 
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::StartUp()
	{
		if (SDL_Init(SDL_INIT_VIDEO) == SDL_ERROR)
		{
			LOG("SDL video subsystem could not be initialized! SDL Error: %s", SDL_GetError());
			return m_isInitOk;
		}

		m_window = SDL_CreateWindow(g_game->GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
		if (m_window == nullptr)
		{
			LOG("Window could not be created! SDL Error: %s", SDL_GetError());
			return m_isInitOk;
		}

		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::ShutDown()
	{
		if (m_isInitOk)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;

			LOG("Quitting SDL video subsystem");
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Window* ModuleWindow::GetWindow() const
	{
		return m_window;
	}
}
