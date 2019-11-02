#ifndef __SINGLETON_INPUT_COMPONENT_H__
#define __SINGLETON_INPUT_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonInputComponent : public NetComponentWrite {

    enum class MemberType {
        ACCELERATION = 1 << 0,
        ANGULAR_ACCELERATION = 1 << 1,

        COUNT,
        INVALID
    };

    SingletonInputComponent()
        : m_acceleration()
        , m_angularAcceleration(0.0f)
    {
    }
    ~SingletonInputComponent()
    {
    }

    void Write(OutputMemoryStream& stream, U16 members) const override
    {
        stream.Write(members, GetRequiredBits<static_cast<U32>(MemberType::COUNT)>::value);
        if (members & static_cast<U32>(MemberType::ACCELERATION)) {
            stream.Write(m_acceleration);
        }
        if (members & static_cast<U32>(MemberType::ANGULAR_ACCELERATION)) {
            stream.Write(m_angularAcceleration);
        }
    }

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
