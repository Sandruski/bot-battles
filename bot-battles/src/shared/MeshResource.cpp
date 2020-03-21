#include "MeshResource.h"

#include "Game.h"
#include "MeshImporter.h"

namespace sand {

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
const std::array<Vertex, 4>& MeshResource::GetVertices()
{
    return m_vertices;
}

//----------------------------------------------------------------------------------------------------
U32 MeshResource::GetVAO() const
{
    return m_VAO;
}
}
