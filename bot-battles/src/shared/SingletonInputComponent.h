#ifndef __SINGLETON_INPUT_COMPONENT_H__
#define __SINGLETON_INPUT_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonInputComponent : public WriteNetComponent, public ReadNetComponent {

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
        , m_inputTime(0.03f)
        , m_lastTime(0.0f)
    {
    }
    ~SingletonInputComponent()
    {
    }

    F32 GetNextInputTime() const
    {
        return m_lastTime + m_inputTime;
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

    void Read(InputMemoryStream& inputStream) override
    {
        U16 memberFlags = 0;
        inputStream.Read(memberFlags, GetRequiredBits<static_cast<U16>(MemberType::COUNT)>::value);
        if (memberFlags & static_cast<U32>(MemberType::ACCELERATION)) {
            inputStream.Read(m_acceleration);
        }
        if (memberFlags & static_cast<U32>(MemberType::ANGULAR_ACCELERATION)) {
            inputStream.Read(m_angularAcceleration);
        }
    }

    Vec2 m_acceleration;
    float m_angularAcceleration;

    F32 m_inputTime;
    F32 m_lastTime;
};
}

#endif
