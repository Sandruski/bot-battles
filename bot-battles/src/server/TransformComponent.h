#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent : public NetComponentWrite {

    enum class MemberType : U16 {
        POSITION = 1 << 0,
        ROTATION = 1 << 1,

        COUNT,
        INVALID,

        ALL = POSITION | ROTATION
    };

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static TransformComponent* Instantiate() { return new TransformComponent(); }

    TransformComponent()
        : m_position()
        , m_rotation(0.0f)
    {
    }
    ~TransformComponent() { }

    void Write(OutputMemoryStream& outputStream, U16 memberFlags) const override
    {
        outputStream.Write(memberFlags, GetRequiredBits<static_cast<U16>(MemberType::COUNT)>::value);
        if (memberFlags & static_cast<U16>(MemberType::POSITION)) {
            outputStream.Write(m_position);
        }
        if (memberFlags & static_cast<U16>(MemberType::ROTATION)) {
            outputStream.Write(m_rotation);
        }
    }

    Vec2 m_position;
    float m_rotation;
};
}

#endif
