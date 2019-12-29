#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

namespace sand {

class FontResource;

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    SDL_Renderer* m_renderer;

    std::weak_ptr<FontResource> m_defaultFont;
    SDL_Color m_backgroundColor;
    bool m_isVsync;
    bool m_isDebugDraw;
};
}

#endif
