#ifndef __WINDOW_COMPONENT_H__
#define __WINDOW_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct WindowComponent {

    WindowComponent();

    SDL_Window* m_window;
    U32 m_width;
    U32 m_height;
};
}

#endif
