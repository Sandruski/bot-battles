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
    bool UpdateVSync() const;
    void UpdateBackgroundColor() const;

    void DrawLine(glm::vec3 fromPosition, glm::vec3 toPosition, glm::vec4 color);
    void DrawCircle(glm::vec3 position, F32 rotation, glm::vec3 scale, U32 sides, F32 angle, F32 radius, glm::vec4 color, bool filled);
    void DrawQuad(glm::vec3 position, F32 rotation, glm::vec3 scale, glm::vec4 color, bool filled);
    void DrawMapTexturedQuad(U32 texture);
    void DrawCharactersTexturedQuad(U32 texture);

    std::weak_ptr<ShaderResource> m_defaultShaderResource;
    std::weak_ptr<ShaderResource> m_instancingShaderResource;

    std::weak_ptr<MeshResource> m_lineMeshResource;
    std::weak_ptr<MeshResource> m_circleMeshResource;
    std::weak_ptr<MeshResource> m_quadMeshResource;

    std::weak_ptr<MeshResource> m_mapMeshResource;
    std::weak_ptr<MeshResource> m_charactersMeshResource;

    glm::vec4 m_backgroundColor;

    bool m_isVSync;
    bool m_isCap;
    bool m_isDebugDraw;
};
}

#endif
