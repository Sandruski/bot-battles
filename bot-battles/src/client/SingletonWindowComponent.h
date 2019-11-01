#ifndef __SINGLETON_WINDOW_COMPONENT_H__
#define __SINGLETON_WINDOW_COMPONENT_H__

#include "ComponentDefs.h"

struct SDL_Window;

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonWindowComponent {
    SingletonWindowComponent()
        : m_window(nullptr)
        , m_width(640)
        , m_height(480)
    {
    }
    ~SingletonWindowComponent() { }

    SDL_Window* m_window;
    U32 m_width;
    U32 m_height;
};
}

#endif
