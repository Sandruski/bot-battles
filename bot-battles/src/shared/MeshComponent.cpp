#include "MeshComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MeshComponent::MeshComponent()
    : m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
{
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
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)(3 * sizeof(F32)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
}
