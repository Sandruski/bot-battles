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

    static const std::vector<Vertex> GetQuadVertices();
    static const std::vector<U32> GetQuadIndices();

public:
    MeshResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;
    bool ForceLoad(const std::vector<Vertex>& vertices, const std::vector<U32>& indices);
    bool ReLoad(const std::vector<Vertex>& vertices);

    U32 GetVAO() const;

private:
    std::vector<Vertex> m_vertices;
    std::vector<U32> m_indices;
    U32 m_VAO;
    U32 m_VBO;
    U32 m_EBO;
};
}

#endif
