#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "ComponentMemberTypes.h"
#include "MemoryStream.h"
#include "NetComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct InputComponent : public NetComponent {

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

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override
    {
        U32 writtenState = 0;

        if (dirtyState & static_cast<U32>(ComponentMemberType::INPUT_ACCELERATION)) {
            outputStream.Write(m_acceleration);
            writtenState |= static_cast<U32>(ComponentMemberType::INPUT_ACCELERATION);
        }
        if (dirtyState & static_cast<U32>(ComponentMemberType::INPUT_ANGULAR_ACCELERATION)) {
            outputStream.Write(m_angularAcceleration);
            writtenState |= static_cast<U32>(ComponentMemberType::INPUT_ANGULAR_ACCELERATION);
        }

        return writtenState;
    }

    void Read(InputMemoryStream& inputStream, U32 dirtyState) override
    {
        if (dirtyState & static_cast<U32>(ComponentMemberType::INPUT_ACCELERATION)) {
            inputStream.Read(m_acceleration);
        }
        if (dirtyState & static_cast<U32>(ComponentMemberType::INPUT_ANGULAR_ACCELERATION)) {
            inputStream.Read(m_angularAcceleration);
        }
    }

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
