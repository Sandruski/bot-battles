#include "InputComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
InputComponent::InputComponent()
    : m_acceleration()
    , m_angularAcceleration(0.0f)
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

    return writtenState;
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION)) {
        inputStream.Read(m_acceleration);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION)) {
        inputStream.Read(m_angularAcceleration);
    }
}
}
