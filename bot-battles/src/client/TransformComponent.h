#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationManager.h"
#include "Vec2.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent : public NetComponent {
    enum class MemberType {
        POSITION = 1 << 0,

        COUNT,
        INVALID
    };

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static TransformComponent* Instantiate() { return new TransformComponent(); }

    TransformComponent()
        : m_position()
    {
    }
    ~TransformComponent() { }

    void Write(OutputMemoryStream& stream, U32 members) const override
    {
        // Serialize
        //stream.Write(members, GetRequiredBits<static_cast<U32>(MemberType::COUNT)>::value);
        if (members & static_cast<U32>(MemberType::POSITION)) {
            stream.WritePosition(m_position);
        }
    }

    void Read(InputMemoryStream& stream) override
    {
        // DeSerialize
        U32 members = 0;
        //stream.Read(members, GetRequiredBits<static_cast<U32>(MemberType::COUNT)>::value);
        if (members & static_cast<U32>(MemberType::POSITION)) {
            stream.ReadPosition(m_position);
        }
    }

    Vec2 m_position;
};
}

#endif
