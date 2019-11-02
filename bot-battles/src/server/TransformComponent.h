#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent : public NetComponentWrite {

    enum class MemberType {
        POSITION = 1 << 0,
        ROTATION = 1 << 1,

        COUNT,
        INVALID
    };

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static TransformComponent* Instantiate() { return new TransformComponent(); }

    TransformComponent()
        : m_position()
        , m_rotation(0.0f)
    {
    }
    ~TransformComponent() { }

    void Write(OutputMemoryStream& stream, U16 members) const override
    {
        stream.Write(members, GetRequiredBits<static_cast<U32>(MemberType::COUNT)>::value);
        if (members & static_cast<U32>(MemberType::POSITION)) {
            stream.Write(m_position);
        }
        if (members & static_cast<U32>(MemberType::ROTATION)) {
            stream.Write(m_rotation);
        }
    }

    Vec2 m_position;
    float m_rotation;
};
}

#endif
