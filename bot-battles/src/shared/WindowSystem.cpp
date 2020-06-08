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

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    U32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    std::string name;
#ifdef _CLIENT
    name = "Bot Battles (Client)";
#elif defined(_SERVER)
    name = "Bot Battles (Server)";
#endif
    windowComponent.lock()->m_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowComponent.lock()->m_currentResolution.x, windowComponent.lock()->m_currentResolution.y, windowFlags);
    if (windowComponent.lock()->m_window == nullptr) {
        ELOG("Window could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WindowSystem::ShutDown()
{
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();

    SDL_DestroyWindow(windowComponent.lock()->m_window);
    windowComponent.lock()->m_window = nullptr;

    ILOG("Quitting SDL video subsystem");
    SDL_QuitSubSystem(SDL_INIT_VIDEO);

    return true;
}
}
