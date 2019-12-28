#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

#include "FontResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();

    SDL_Renderer* m_renderer;
    std::shared_ptr<FontResource> m_font;
    SDL_Color m_backgroundColor;
    bool m_isDebugDraw;
};
}

#endif
