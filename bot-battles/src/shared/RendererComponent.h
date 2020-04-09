#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

namespace sand {

class ShaderResource;
class MeshResource;

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void SetWireframe(bool wireframe) const;
    void UpdateBackgroundColor();

    std::weak_ptr<ShaderResource> m_shaderResource;
    std::weak_ptr<MeshResource> m_meshResource;

    glm::vec4 m_backgroundColor;

    bool m_isVsync;

    bool m_isDebugDraw;
};
}

#endif
