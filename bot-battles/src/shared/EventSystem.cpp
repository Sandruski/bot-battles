#include "EventSystem.h"

#include "EventComponent.h"
#include "Game.h"

namespace sand {

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
    OPTICK_EVENT();

    EventComponent& eventComponent = g_game->GetEventComponent();

    const U8* keyboardState = SDL_GetKeyboardState(nullptr);
    for (U16 i = 0; i < SDL_NUM_SCANCODES; ++i) {
        EventComponent::KeyState& keyState = eventComponent.m_keyboard.at(i);
        if (keyboardState[i] == SDL_KEY_PRESSED) {
            if (keyState == EventComponent::KeyState::IDLE) {
                keyState = EventComponent::KeyState::DOWN;
            } else {
                keyState = EventComponent::KeyState::REPEAT;
            }
        } else {
            if (keyState == EventComponent::KeyState::DOWN
                || keyState == EventComponent::KeyState::REPEAT) {
                keyState = EventComponent::KeyState::UP;
            } else {
                keyState = EventComponent::KeyState::IDLE;
            }
        }
    }

    U32 mouseState = SDL_GetMouseState(&eventComponent.m_mousePosition.x, &eventComponent.m_mousePosition.y);
    for (U8 i = 0; i < SDL_BUTTON_X2; ++i) {
        EventComponent::KeyState& keyState = eventComponent.m_mouse.at(i);
        if (mouseState & SDL_BUTTON(i + SDL_BUTTON_LEFT)) {
            if (keyState == EventComponent::KeyState::IDLE) {
                keyState = EventComponent::KeyState::DOWN;
            } else {
                keyState = EventComponent::KeyState::REPEAT;
            }
        } else {
            if (keyState == EventComponent::KeyState::DOWN
                || keyState == EventComponent::KeyState::REPEAT) {
                keyState = EventComponent::KeyState::UP;
            } else {
                keyState = EventComponent::KeyState::IDLE;
            }
        }
    }

    static SDL_Event event;
    while (SDL_PollEvent(&event)) {

        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type) {

        case SDL_QUIT: {
            return false;
            break;
        }

        case SDL_WINDOWEVENT: {
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SHOWN:
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
                g_game->m_isRunning = true;
                break;
            }

            case SDL_WINDOWEVENT_HIDDEN:
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_FOCUS_LOST: {
                g_game->m_isRunning = false;
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
