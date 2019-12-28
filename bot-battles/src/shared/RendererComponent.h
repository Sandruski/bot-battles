#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

namespace sand {

class FontResource;

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();

    SDL_Renderer* m_renderer;
    std::weak_ptr<FontResource> m_font;
    SDL_Color m_backgroundColor;
    bool m_isDebugDraw;
};
}

#endif
