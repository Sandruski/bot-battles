#include "MeshComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MeshComponent::MeshComponent()
    : m_vertices()
    , m_indices()
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
{
    // Vertices
    // Top-left
    m_vertices[0].m_position = glm::vec2(-0.5f, 0.5f);
    m_vertices[0].m_textureCoords = glm::vec2(0.0f, 1.0f);
    // Top-right
    m_vertices[1].m_position = glm::vec2(0.5f, 0.5f);
    m_vertices[1].m_textureCoords = glm::vec2(1.0f, 1.0f);
    // Bottom-left
    m_vertices[2].m_position = glm::vec2(-0.5f, -0.5f);
    m_vertices[2].m_textureCoords = glm::vec2(0.0f, 0.0f);
    // Bottom-right
    m_vertices[3].m_position = glm::vec2(0.5f, -0.5f);
    m_vertices[3].m_textureCoords = glm::vec2(1.0f, 0.0f);

    // Indices
    // First triangle
    m_indices[0] = 0;
    m_indices[1] = 2;
    m_indices[2] = 3;
    // Second triangle
    m_indices[3] = 3;
    m_indices[4] = 1;
    m_indices[5] = 0;
}

//----------------------------------------------------------------------------------------------------
MeshComponent::~MeshComponent()
{
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void MeshComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    Init();
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 MeshComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    U32 writtenState = 0;
    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
void MeshComponent::Init()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    UpdateVertex();

    // Positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::m_position)));
    glEnableVertexAttribArray(0);

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::m_textureCoords)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void MeshComponent::UpdateVertex()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices[0], GL_DYNAMIC_DRAW);
}
}
