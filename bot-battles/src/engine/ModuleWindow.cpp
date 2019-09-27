#include "ModuleWindow.h"
#include "Engine.h"

#include "Utils.h"
#include <SDL.h>
#include <stdio.h>

// uncomment to disable assert()
// #define NDEBUG

#include <cassert>

const U32 SCREEN_WIDTH = 640;
const U32 SCREEN_HEIGHT = 480;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleWindow::ModuleWindow() : m_window(nullptr), m_isInitOk(false) {}

	//----------------------------------------------------------------------------------------------------
	ModuleWindow::~ModuleWindow() {}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::StartUp()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			return m_isInitOk;
		}

		m_window = SDL_CreateWindow(g_engine->GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (m_window == nullptr)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_Quit();
			return m_isInitOk;
		}

		SDL_Surface* screenSurface = SDL_GetWindowSurface(m_window);
		SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
		SDL_UpdateWindowSurface(m_window);

		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::ShutDown()
	{
		bool ret = m_isInitOk;

		if (ret)
		{
			CleanUp();
		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::CleanUp()
	{
		SDL_DestroyWindow(m_window);
		SDL_Quit();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	ModuleType ModuleWindow::GetType() const
	{
		return ModuleType::WINDOW;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleWindow::IsActive() const
	{
		return m_isActive;
	}
}
