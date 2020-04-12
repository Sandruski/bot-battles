#ifndef __LABEL_COMPONENT_H__
#define __LABEL_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct LabelComponent : public Component
#ifdef _CLIENT
    ,
                        public NetworkableReadObject
#elif defined(_SERVER)
    ,
                        public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::LABEL; }

    LabelComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    // Networked
    glm::vec2 m_offset;
    std::string m_text;
    glm::vec4 m_color;
};
}

#endif