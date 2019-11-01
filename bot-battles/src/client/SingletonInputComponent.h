#ifndef __SINGLETON_INPUT_COMPONENT_H__
#define __SINGLETON_INPUT_COMPONENT_H__

#include "ComponentDefs.h"

#include "Vec2I.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonInputComponent {
    enum class KeyState {
        IDLE,
        DOWN,
        REPEAT,
        UP,

        COUNT,
        INVALID
    };

    SingletonInputComponent()
        : m_keyboard()
        , m_mouse()
        , m_mousePosition()
    {
        memset(m_keyboard, static_cast<int>(KeyState::IDLE), sizeof(KeyState) * SDL_NUM_SCANCODES);
        memset(m_mouse, static_cast<int>(KeyState::IDLE), sizeof(KeyState) * SDL_BUTTON_X2);
    }
    ~SingletonInputComponent() { }

    KeyState GetKey(U32 key);
    KeyState GetMouseButton(U8 button);

    KeyState m_keyboard[SDL_NUM_SCANCODES];
    KeyState m_mouse[SDL_BUTTON_X2];
    Vec2I m_mousePosition;
};
}

#endif
