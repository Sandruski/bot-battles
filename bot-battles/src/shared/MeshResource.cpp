#include "MeshResource.h"

#include "Game.h"
#include "MeshImporter.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const std::vector<MeshResource::Vertex> MeshResource::GetLineVertices(glm::vec3 fromPosition, glm::vec3 toPosition)
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    U32 verticesCount = 2;

    std::vector<Vertex> vertices;
    vertices.reserve(verticesCount);

    Vertex vertex = Vertex();
    vertex.m_position = fromPosition;
    vertex.m_position.x *= proportion.x;
    vertex.m_position.y *= proportion.y;
    vertex.m_position.y *= -1.0f;
    vertices.emplace_back(vertex);

    vertex.m_position = toPosition;
    vertex.m_position.x *= proportion.x;
    vertex.m_position.y *= proportion.y;
    vertex.m_position.y *= -1.0f;
    vertices.emplace_back(vertex);

    return vertices;
}

//----------------------------------------------------------------------------------------------------
const std::vector<MeshResource::Vertex> MeshResource::GetCircleVertices(U32 sides, F32 angle, F32 radius)
{
    U32 verticesCount = sides + 2;

    std::vector<Vertex> vertices;
    vertices.reserve(verticesCount);

    Vertex vertex = Vertex();
    vertices.emplace_back(vertex);

    F32 increment = glm::radians(angle) / static_cast<F32>(sides);
    for (U32 i = 0; i < verticesCount - 1; ++i) {
        vertex.m_position.x = radius * glm::cos(i * increment);
        vertex.m_position.y = radius * glm::sin(i * increment);
        vertices.emplace_back(vertex);
    }

    return vertices;
}

//----------------------------------------------------------------------------------------------------
const std::vector<MeshResource::Vertex> MeshResource::GetQuadVertices()
{
    U32 verticesCount = 4;

    std::vector<Vertex> vertices;
    vertices.reserve(verticesCount);

    Vertex vertex = Vertex();
    vertex.m_position = glm::vec2(-0.5f, 0.5f);
    vertex.m_textureCoords = glm::vec2(0.0f, 1.0f);
    vertices.emplace_back(vertex);

    vertex.m_position = glm::vec2(0.5f, 0.5f);
    vertex.m_textureCoords = glm::vec2(1.0f, 1.0f);
    vertices.emplace_back(vertex);

    vertex.m_position = glm::vec2(-0.5f, -0.5f);
    vertex.m_textureCoords = glm::vec2(0.0f, 0.0f);
    vertices.emplace_back(vertex);

    vertex.m_position = glm::vec2(0.5f, -0.5f);
    vertex.m_textureCoords = glm::vec2(1.0f, 0.0f);
    vertices.emplace_back(vertex);

    return vertices;
}

//----------------------------------------------------------------------------------------------------
MeshResource::MeshResource(U32 id, const char* dir, const char* file)
    : Resource(id, dir, file)
    , m_vertices()
    , m_VAO(0)
    , m_VBO(0)
    , m_instanceVBO(0)
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
    if (m_VAO > 0 && m_VBO > 0) {
        g_game->GetMeshImporter().UnLoad(m_VAO, m_VBO, m_instanceVBO);
    }

    return (m_VAO == 0 && m_VBO == 0);
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::ForceLoad(const std::vector<Vertex>& vertices)
{
    UnLoad();

    g_game->GetMeshImporter().Load(m_VAO, m_VBO, m_instanceVBO);
    g_game->GetMeshImporter().ReLoad(vertices, m_VBO);

    const bool isLoaded = (m_VAO > 0 && m_VBO > 0);
    if (isLoaded) {
        m_vertices = vertices;
    }

    return isLoaded;
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::ReLoad(const std::vector<Vertex>& vertices)
{
    assert(m_VAO > 0 && m_VBO > 0);

    m_vertices = vertices;

    g_game->GetMeshImporter().ReLoad(vertices, m_VBO);

    return (m_VAO > 0 && m_VBO > 0);
}

//----------------------------------------------------------------------------------------------------
bool MeshResource::ReLoadInstance(const std::vector<Instance>& instances)
{
    assert(m_VAO > 0 && m_instanceVBO > 0);

    g_game->GetMeshImporter().ReLoadInstance(instances, m_instanceVBO);

    return (m_VAO > 0 && m_instanceVBO > 0);
}

//----------------------------------------------------------------------------------------------------
const std::vector<MeshResource::Vertex>& MeshResource::GetVertices() const
{
    return m_vertices;
}

//----------------------------------------------------------------------------------------------------
U32 MeshResource::GetVAO() const
{
    return m_VAO;
}
}
