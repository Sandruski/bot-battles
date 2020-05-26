#ifndef __MESH_RESOURCE_H__
#define __MESH_RESOURCE_H__

#include "Resource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MeshResource : public Resource {
public:
    struct Vertex {

        Vertex()
            : m_position(0.0f, 0.0f)
            , m_textureCoords(0.0f, 0.0f)
        {
        }

        glm::vec2 m_position;
        glm::vec2 m_textureCoords;
    };

    struct Instance {

        Instance()
            : m_spriteCoords()
            , m_model(0.0f)
        {
        }

        glm::vec2 m_spriteCoords[4];
        glm::mat4 m_model;
    };

    static const std::vector<Vertex> GetLineVertices(glm::vec3 fromPosition, glm::vec3 toPosition);
    static const std::vector<Vertex> GetCircleVertices(U32 sides, F32 angle, F32 radius);
    static const std::vector<Vertex> GetQuadVertices();

public:
    MeshResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;
    bool ForceLoad();
    bool ReLoadVertices(const std::vector<Vertex>& vertices);
    bool ReLoadInstances(const std::vector<Instance>& instances);

    const std::vector<Vertex>& GetVertices() const;
    U32 GetVAO() const;

private:
    std::vector<Vertex> m_vertices;
    U32 m_VAO;
    U32 m_VBO;
    U32 m_instanceVBO;
};
}

#endif
