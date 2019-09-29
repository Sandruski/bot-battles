#include "ModuleWindow.h"
#include "Engine.h"

#include "Log.h"
#include "Memory.h"

#include <SDL.h>
#include <stdio.h>

const U32 SCREEN_WIDTH = 640;
const U32 SCREEN_HEIGHT = 480;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleWindow::ModuleWindow() : 
		m_window(nullptr),
		m_surface(nullptr),
		m_isInitOk(false) 
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleWindow::~ModuleWindow() 
	{
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleWindow::GetName() const
	{
		return "Window";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::StartUp()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			LOG("SDL could not be initialized! SDL Error: %s", SDL_GetError());
			return m_isInitOk;
		}

		m_window = SDL_CreateWindow(g_engine->GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (m_window == nullptr)
		{
			LOG("Window could not be created! SDL Error: %s", SDL_GetError());
			SDL_Quit();
			return m_isInitOk;
		}

		m_surface = SDL_GetWindowSurface(m_window);
		//SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
		//SDL_UpdateWindowSurface(m_window);

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

			SDL_Quit();
		}

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Window* ModuleWindow::GetWindow() const
	{
		return m_window;
	}
}
