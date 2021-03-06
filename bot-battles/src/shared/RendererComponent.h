#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

namespace sand {

class ShaderResource;
class MeshResource;

//----------------------------------------------------------------------------------------------------
// System Component
struct RendererComponent {

    RendererComponent();
    ~RendererComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void SetWireframe(bool wireframe) const;
    bool UpdateVSync() const;
    void UpdateBackgroundColor() const;

    void DrawLine(const glm::vec2& fromPosition, const glm::vec2& toPosition, F32 positionZ, const glm::vec4& color);
    void DrawCircle(const glm::vec3& position, F32 rotation, const glm::vec3& scale, U32 sides, F32 angle, F32 radius, const glm::vec4& color, bool filled);
    void DrawQuad(const glm::vec3& position, F32 rotation, const glm::vec3& scale, const glm::vec4& color, bool filled);
    void DrawMapTexturedQuad(U32 texture);
    void DrawCharactersTexturedQuad(U32 texture);
    void DrawObjectsTexturedQuad(U32 texture);
    void DrawBackgroundTexturedQuad(U32 texture);

    std::weak_ptr<ShaderResource> m_defaultShaderResource;
    std::weak_ptr<ShaderResource> m_instancingShaderResource;

    std::weak_ptr<MeshResource> m_lineMeshResource;
    std::weak_ptr<MeshResource> m_circleMeshResource;
    std::weak_ptr<MeshResource> m_quadMeshResource;
    std::weak_ptr<MeshResource> m_mapMeshResource;
    std::weak_ptr<MeshResource> m_charactersMeshResource;
    std::weak_ptr<MeshResource> m_objectsMeshResource;
    std::weak_ptr<MeshResource> m_backgroundMeshResource;

    glm::vec4 m_backgroundColor;

    std::string m_mainMenuTextureName;
    std::string m_scoreboardTextureName;

    bool m_isVSync;
    bool m_isCap;
    bool m_isDebugDraw;
    bool m_isDebugDrawColliders;
    bool m_isDebugDrawBot;
    bool m_isDebugDrawMap;
};
}

#endif
