#include "InputSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystem::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.IsConnected()) {
        return true;
    }

    const U8* keyboardState = SDL_GetKeyboardState(nullptr);
    for (U16 i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (keyboardState[i] == SDL_KEY_PRESSED) {
            if (m_keyboard[i] == KeyState::IDLE) {
                m_keyboard[i] = KeyState::DOWN;
            } else {
                m_keyboard[i] = KeyState::REPEAT;
            }
        } else {
            if (m_keyboard[i] == KeyState::DOWN
                || m_keyboard[i] == KeyState::REPEAT) {
                m_keyboard[i] = KeyState::UP;
            } else {
                m_keyboard[i] = KeyState::IDLE;
            }
        }
    }

    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    inputComponent.m_acceleration = Vec2::zero;
    const F32 multiplier = 5.0f;
    if (m_keyboard[SDL_SCANCODE_A] == KeyState::REPEAT) {
        inputComponent.m_acceleration.x = -1.0f * multiplier;
    }
    if (m_keyboard[SDL_SCANCODE_D] == KeyState::REPEAT) {
        inputComponent.m_acceleration.x = 1.0f * multiplier;
    }
    if (m_keyboard[SDL_SCANCODE_W] == KeyState::REPEAT) {
        inputComponent.m_acceleration.y = -1.0f * multiplier;
    }
    if (m_keyboard[SDL_SCANCODE_S] == KeyState::REPEAT) {
        inputComponent.m_acceleration.y = 1.0f * multiplier;
    }

    if (inputComponent.m_acceleration.x != 0.0f || inputComponent.m_acceleration.y != 0.0f) {
        if (!clientComponent.m_inputBuffer.IsFull()) {
            ILOG("CLIENT NEW FRAME!!!");
            U32 dirtyState = static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION); // TODO: not only acceleration...
            Input input = Input(inputComponent, dirtyState, 1.0f, clientComponent.m_inputBuffer.m_back);
            clientComponent.m_inputBuffer.Add(input);
            clientComponent.m_isLastMovePending = true;
        }
    }

    return true;
}
}
