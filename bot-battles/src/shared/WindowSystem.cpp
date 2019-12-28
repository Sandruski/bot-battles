#include "WindowSystem.h"

#include "Game.h"

#include "WindowComponent.h"

namespace sand {

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
    if (SDL_Init(SDL_INIT_VIDEO) == SDL_ERROR) {
        ELOG("SDL video subsystem could not be initialized! SDL Error: %s", SDL_GetError());
        return false;
    }

    WindowComponent& windowComponent = g_game->GetWindowComponent();

    windowComponent.m_window = SDL_CreateWindow(g_game->GetName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowComponent.m_width, windowComponent.m_height, SDL_WINDOW_SHOWN);
    if (windowComponent.m_window == nullptr) {
        ELOG("Window could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WindowSystem::ShutDown()
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();

    SDL_DestroyWindow(windowComponent.m_window);
    windowComponent.m_window = nullptr;

    ILOG("Quitting SDL video subsystem");
    SDL_QuitSubSystem(SDL_INIT_VIDEO);

    return true;
}
}
