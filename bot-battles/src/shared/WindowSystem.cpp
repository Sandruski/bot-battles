#include "WindowSystem.h"

#include "Config.h"
#include "Game.h"
#include "GuiComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool WindowSystem::StartUp()
{
    if (SDL_Init(SDL_INIT_VIDEO) == SDL_ERROR) {
        ELOG("SDL video subsystem could not be initialized! SDL Error: %s", SDL_GetError());
        return false;
    }

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    U32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    glm::uvec2 resolution = windowComponent.GetResolution();
    windowComponent.m_window = SDL_CreateWindow(g_game->GetConfig().m_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, flags);
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
