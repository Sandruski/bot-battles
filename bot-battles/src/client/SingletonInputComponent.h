#ifndef __SINGLETON_INPUT_COMPONENT_H__
#define __SINGLETON_INPUT_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonInputComponent : public WriteNetComponent {

    enum class MemberType {
        ACCELERATION = 1 << 0,
        ANGULAR_ACCELERATION = 1 << 1,

        COUNT,
        INVALID,

        ALL = ACCELERATION | ANGULAR_ACCELERATION
    };

    SingletonInputComponent()
        : m_acceleration()
        , m_angularAcceleration(0.0f)
    {
    }
    ~SingletonInputComponent()
    {
    }

    void Write(OutputMemoryStream& outputStream, U16 memberFlags) const override
    {
        outputStream.Write(memberFlags, GetRequiredBits<static_cast<U16>(MemberType::COUNT)>::value);
        if (memberFlags & static_cast<U16>(MemberType::ACCELERATION)) {
            outputStream.Write(m_acceleration);
        }
        if (memberFlags & static_cast<U16>(MemberType::ANGULAR_ACCELERATION)) {
            outputStream.Write(m_angularAcceleration);
        }
    }

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
