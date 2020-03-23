#ifndef __EVENT_COMPONENT_H__
#define __EVENT_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct EventComponent {

    enum class KeyState {
        IDLE,
        DOWN,
        REPEAT,
        UP,

        COUNT,
        INVALID
    };

    EventComponent();

    std::array<KeyState, SDL_NUM_SCANCODES> m_keyboard;
    std::array<KeyState, SDL_BUTTON_X2> m_mouse;
    glm::ivec2 m_mousePosition;
};
}

#endif
