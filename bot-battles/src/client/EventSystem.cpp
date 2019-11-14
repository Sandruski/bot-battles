#include "EventSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
EventSystem::EventSystem()
{
}

//----------------------------------------------------------------------------------------------------
EventSystem::~EventSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool EventSystem::StartUp()
{
    if (SDL_Init(SDL_INIT_EVENTS) == SDL_ERROR) {
        ELOG("SDL events subsystem could not be initialized! SDL Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool EventSystem::Update()
{
    static SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: {
            // TODO SEND EVENT
            return false;
            break;
        }

        case SDL_WINDOWEVENT: {
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SHOWN:
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
                // TODO: we show and update m_isRunning accordingly
                // TODO: Event System

                break;
            }

            case SDL_WINDOWEVENT_HIDDEN:
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_FOCUS_LOST: {
                // TODO: we hide and update m_isRunning accordingly
                // TODO: Event System

                break;
            }

            default: {
                break;
            }
            }
            break;
        }

        default: {
            break;
        }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool EventSystem::ShutDown()
{
    ILOG("Quitting SDL events subsystem");
    SDL_QuitSubSystem(SDL_INIT_EVENTS);

    return true;
}
}
