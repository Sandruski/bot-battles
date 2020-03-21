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

public:
    MeshResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;
    bool ForceLoad(const std::array<Vertex, 4>& vertices, const std::array<U32, 6>& indices);
    bool ReLoad(const std::array<Vertex, 4>& vertices);

    const std::array<Vertex, 4>& GetVertices();
    U32 GetVAO() const;

private:
    std::array<Vertex, 4> m_vertices;
    std::array<U32, 6> m_indices;
    U32 m_VAO;
    U32 m_VBO;
    U32 m_EBO;
};
}

#endif
