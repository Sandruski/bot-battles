#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

namespace sand {

class ShaderResource;

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void UpdateBackgroundColor();

    std::weak_ptr<ShaderResource> m_shaderResource;
    glm::vec4 m_backgroundColor;
    bool m_isVsync;
    bool m_isDebugDraw;
};
}

#endif
