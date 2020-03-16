#ifndef __MESH_COMPONENT_H__
#define __MESH_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct MeshComponent : public Component
#ifdef _CLIENT
    ,
                       public NetworkableReadObject
#elif defined(_SERVER)
    ,
                       public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::MESH; }

    struct Vertex {

        Vertex()
            : m_position(0.0f, 0.0f)
            , m_textureCoords(0.0f, 0.0f)
        {
        }

        glm::vec2 m_position;
        glm::vec2 m_textureCoords;
    };

    MeshComponent();
    ~MeshComponent() override;

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    void Init();
    void UpdateVertex();

    std::array<Vertex, 4> m_vertices;
    std::array<U32, 6> m_indices;
    U32 m_VAO;
    U32 m_VBO;
    U32 m_EBO;
};
}

#endif
