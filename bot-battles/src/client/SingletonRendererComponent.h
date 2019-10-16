#ifndef __SINGLETON_RENDERER_COMPONENT_H__
#define __SINGLETON_RENDERER_COMPONENT_H__

#include "ComponentDefs.h"

#include <SDL_pixels.h>

struct SDL_Renderer;

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonRendererComponent {
    static SingletonComponentType GetType() { return SingletonComponentType::RENDERER; }

	SingletonRendererComponent()
        : m_renderer(nullptr)
        , m_backgroundColor()
        , m_isDebugDraw(false)
    {
    }
    ~SingletonRendererComponent() { }

    SDL_Renderer* m_renderer;
    SDL_Color m_backgroundColor;
    bool m_isDebugDraw;
};
}

#endif