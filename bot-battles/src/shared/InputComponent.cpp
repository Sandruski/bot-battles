#include "InputComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
InputComponent::InputComponent()
    : m_maxAcceleration(300.0f)
    , m_maxAngularAcceleration(200.0f)
    , m_acceleration(0.0f, 0.0f)
    , m_angularAcceleration(0.0f)
    , m_isShooting(false)
{
}

//----------------------------------------------------------------------------------------------------
U32 InputComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION)) {
        outputStream.Write(m_acceleration);
        writtenState |= static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION)) {
        outputStream.Write(m_angularAcceleration);
        writtenState |= static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING)) {
        outputStream.Write(m_isShooting);
        writtenState |= static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
    }

    return writtenState;
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION)) {
        inputStream.Read(m_acceleration);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION)) {
        inputStream.Read(m_angularAcceleration);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING)) {
        inputStream.Read(m_isShooting);
    }
}
}
