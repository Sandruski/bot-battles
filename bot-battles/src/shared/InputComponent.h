#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct InputComponent : public WriteNetComponent, public ReadNetComponent {

    enum class MemberType {
        ACCELERATION = 1 << 0,
        ANGULAR_ACCELERATION = 1 << 1,

        COUNT,
        INVALID,

        ALL = ACCELERATION | ANGULAR_ACCELERATION
    };

    static ComponentType GetType() { return ComponentType::INPUT; }
    static InputComponent* Instantiate() { return new InputComponent(); }

    InputComponent()
        : m_acceleration()
        , m_angularAcceleration(0.0f)
    {
    }
    ~InputComponent() { }

    void Copy(const InputComponent& other)
    {
        this->m_acceleration = other.m_acceleration;
        this->m_angularAcceleration = other.m_angularAcceleration;
    }

    void Write(OutputMemoryStream& outputStream, U16 memberFlags) const override
    {
        outputStream.Write(memberFlags, GetRequiredBits<static_cast<U16>(InputComponent::MemberType::COUNT)>::value);
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
        inputStream.Read(memberFlags, GetRequiredBits<static_cast<U16>(InputComponent::MemberType::COUNT)>::value);
        if (memberFlags & static_cast<U16>(MemberType::ACCELERATION)) {
            inputStream.Read(m_acceleration);
        }
        if (memberFlags & static_cast<U16>(MemberType::ANGULAR_ACCELERATION)) {
            inputStream.Read(m_angularAcceleration);
        }
    }

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
