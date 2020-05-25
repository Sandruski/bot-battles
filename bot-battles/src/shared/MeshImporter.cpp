#include "MeshImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void MeshImporter::Load(const std::vector<MeshResource::Vertex>& /*vertices*/, U32& VAO, U32& VBO, U32& instanceVBO) const
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(MeshResource::Vertex) * vertices.size(), &vertices.at(0), GL_STATIC_DRAW); // TODO: remove

    // Positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Vertex), (void*)(offsetof(MeshResource::Vertex, MeshResource::Vertex::m_position)));
    glEnableVertexAttribArray(0);

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Vertex), (void*)(offsetof(MeshResource::Vertex, MeshResource::Vertex::m_textureCoords)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    // Sprite coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Instance), (void*)(offsetof(MeshResource::Instance, MeshResource::Instance::m_spriteCoords)));
    glEnableVertexAttribArray(2);

    // Model
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Instance), (void*)(offsetof(MeshResource::Instance, MeshResource::Instance::m_model) + sizeof(float) * 0));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Instance), (void*)(offsetof(MeshResource::Instance, MeshResource::Instance::m_model) + sizeof(float) * 4));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Instance), (void*)(offsetof(MeshResource::Instance, MeshResource::Instance::m_model) + sizeof(float) * 8));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(MeshResource::Instance), (void*)(offsetof(MeshResource::Instance, MeshResource::Instance::m_model) + sizeof(float) * 12));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void MeshImporter::ReLoad(const std::vector<MeshResource::Vertex>& vertices, U32 VBO) const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MeshResource::Vertex) * vertices.size(), &vertices.at(0), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------------------------------------------------------------------------
void MeshImporter::ReLoadInstance(const std::vector<MeshResource::Instance>& instances, U32 instanceVBO) const
{
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MeshResource::Instance) * instances.size(), &instances.at(0), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------------------------------------------------------------------------
void MeshImporter::UnLoad(U32& VAO, U32& VBO, U32& instanceVBO) const
{
    glDeleteBuffers(1, &VBO);
    VBO = 0;
    glDeleteBuffers(1, &instanceVBO);
    instanceVBO = 0;
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
}
}
