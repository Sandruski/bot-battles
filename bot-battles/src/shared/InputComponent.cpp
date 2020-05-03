#include "InputComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
InputComponent::InputComponent()
    : m_maxLinearVelocity(300.0f)
    , m_maxAngularVelocity(120.0f)
    , m_linearVelocity(0.0f, 0.0f)
    , m_angularVelocity(0.0f)
    , m_isShooting(false)
{
}

//----------------------------------------------------------------------------------------------------
U32 InputComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY)) {
        outputStream.Write(m_linearVelocity);
        writtenState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY)) {
        outputStream.Write(m_angularVelocity);
        writtenState |= static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
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
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY)) {
        inputStream.Read(m_linearVelocity);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY)) {
        inputStream.Read(m_angularVelocity);
    }
    if (dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING)) {
        inputStream.Read(m_isShooting);
    }
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Shoot()
{
    m_isShooting = true;
}
}
