#include "MeshResource.h"

#include "Game.h"
#include "MeshImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const std::array<MeshResource::Vertex, 4> MeshResource::GetQuadVertices()
{
    std::array<MeshResource::Vertex, 4> vertices;

    // Top-left
    vertices[0].m_position = glm::vec2(-0.5f, 0.5f);
    vertices[0].m_textureCoords = glm::vec2(0.0f, 1.0f);
    // Top-right
    vertices[1].m_position = glm::vec2(0.5f, 0.5f);
    vertices[1].m_textureCoords = glm::vec2(1.0f, 1.0f);
    // Bottom-left
    vertices[2].m_position = glm::vec2(-0.5f, -0.5f);
    vertices[2].m_textureCoords = glm::vec2(0.0f, 0.0f);
    // Bottom-right
    vertices[3].m_position = glm::vec2(0.5f, -0.5f);
    vertices[3].m_textureCoords = glm::vec2(1.0f, 0.0f);

    return vertices;
}

//----------------------------------------------------------------------------------------------------
const std::array<U32, 6> MeshResource::GetQuadIndices()
{
    std::array<U32, 6> indices;

    // First triangle
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 3;
    // Second triangle
    indices[3] = 3;
    indices[4] = 1;
    indices[5] = 0;

    return indices;
}

//----------------------------------------------------------------------------------------------------
MeshResource::MeshResource(U32 id, const char* dir, const char* file)
    : Resource(id, dir, file)
    , m_vertices()
    , m_indices()
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
{
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::Load()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::UnLoad()
{
    if (m_VAO > 0 && m_VBO > 0 && m_EBO > 0) {
        g_game->GetMeshImporter().UnLoad(m_VAO, m_VBO, m_EBO);
    }

    return (m_VAO == 0 && m_VBO == 0 && m_EBO == 0);
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::ForceLoad(const std::array<Vertex, 4>& vertices, const std::array<U32, 6>& indices)
{
    UnLoad();

    g_game->GetMeshImporter().Load(vertices, indices, m_VAO, m_VBO, m_EBO);

    const bool isLoaded = (m_VAO > 0 && m_VBO > 0 && m_EBO > 0);
    if (isLoaded) {
        m_vertices = vertices;
        m_indices = indices;
    }

    return isLoaded;
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::ReLoad(const std::array<Vertex, 4>& vertices)
{
    assert(m_VAO > 0 && m_VBO > 0 && m_EBO > 0);

    m_vertices = vertices;

    g_game->GetMeshImporter().ReLoad(vertices, m_VBO);

    return (m_VAO > 0 && m_VBO > 0 && m_EBO > 0);
}

//----------------------------------------------------------------------------------------------------
U32 MeshResource::GetVAO() const
{
    return m_VAO;
}
}
