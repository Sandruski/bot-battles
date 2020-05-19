#include "MeshResource.h"

#include "Game.h"
#include "MeshImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const std::vector<MeshResource::Vertex> MeshResource::GetQuadVertices()
{
    std::vector<MeshResource::Vertex> vertices;
    vertices.reserve(4);

    MeshResource::Vertex vertex;
    // Top-left
    vertex.m_position = glm::vec2(-0.5f, 0.5f);
    vertex.m_textureCoords = glm::vec2(0.0f, 1.0f);
    vertices.emplace_back(vertex);
    // Top-right
    vertex.m_position = glm::vec2(0.5f, 0.5f);
    vertex.m_textureCoords = glm::vec2(1.0f, 1.0f);
    vertices.emplace_back(vertex);
    // Bottom-left
    vertex.m_position = glm::vec2(-0.5f, -0.5f);
    vertex.m_textureCoords = glm::vec2(0.0f, 0.0f);
    vertices.emplace_back(vertex);
    // Bottom-right
    vertex.m_position = glm::vec2(0.5f, -0.5f);
    vertex.m_textureCoords = glm::vec2(1.0f, 0.0f);
    vertices.emplace_back(vertex);

    return vertices;
}

//----------------------------------------------------------------------------------------------------
const std::vector<U32> MeshResource::GetQuadIndices()
{
    std::vector<U32> indices;
    indices.reserve(6);

    // First triangle
    indices.emplace_back(0);
    indices.emplace_back(2);
    indices.emplace_back(3);
    // Second triangle
    indices.emplace_back(3);
    indices.emplace_back(1);
    indices.emplace_back(0);

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
bool MeshResource::ForceLoad(const std::vector<Vertex>& vertices, const std::vector<U32>& indices)
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
bool MeshResource::ReLoad(const std::vector<Vertex>& vertices)
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
