#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void UpdateBackgroundColor();

    U32 m_shader;
    glm::vec4 m_backgroundColor;
    bool m_isVsync;
    bool m_isDebugDraw;
};
}

#endif
