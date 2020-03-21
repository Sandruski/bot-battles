#include "MeshImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void MeshImporter::Load(const std::array<Vertex, 4>& vertices, const std::array<U32, 6>& indices, U32& VAO, U32& VBO, U32& EBO) const
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::m_position)));
    glEnableVertexAttribArray(0);

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::m_textureCoords)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void MeshImporter::ReLoad(const std::array<Vertex, 4>& vertices, U32 VBO) const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
}

//----------------------------------------------------------------------------------------------------
void MeshImporter::UnLoad(U32& VAO, U32& VBO, U32& EBO) const
{
    glDeleteBuffers(1, &EBO);
    EBO = 0;
    glDeleteBuffers(1, &VBO);
    VBO = 0;
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
}
}
