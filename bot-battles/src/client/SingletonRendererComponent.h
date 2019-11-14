#ifndef __SINGLETON_RENDERER_COMPONENT_H__
#define __SINGLETON_RENDERER_COMPONENT_H__

struct SDL_Renderer;

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonRendererComponent {

    SingletonRendererComponent()
        : m_renderer(nullptr)
        , m_backgroundColor()
        , m_isDebugDraw(true)
    {
    }
    ~SingletonRendererComponent() { }

    SDL_Renderer* m_renderer;
    SDL_Color m_backgroundColor;
    bool m_isDebugDraw;
};
}

#endif
