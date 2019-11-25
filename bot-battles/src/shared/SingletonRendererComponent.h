#ifndef __SINGLETON_RENDERER_COMPONENT_H__
#define __SINGLETON_RENDERER_COMPONENT_H__

#include "FontResource.h"

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
	std::shared_ptr<FontResource> m_font;
    SDL_Color m_backgroundColor;
    bool m_isDebugDraw;
};
}

#endif
