#include "InputComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
InputComponent::InputComponent()
    : m_maxLinearVelocity(100.0f)
    , m_maxAngularVelocity(45.0f)
    , m_linearVelocity(0.0f, 0.0f)
    , m_angularVelocity(0.0f)
    , m_dirtyState(0)
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
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Reset()
{
    m_dirtyState &= ~static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
}

//----------------------------------------------------------------------------------------------------
F32 InputComponent::GetMaxLinearVelocity() const
{
    return m_maxLinearVelocity;
}

//----------------------------------------------------------------------------------------------------
F32 InputComponent::GetMaxAngularVelocity() const
{
    return m_maxAngularVelocity;
}

//----------------------------------------------------------------------------------------------------
void InputComponent::SetLinearVelocityX(F32 linearVelocityX)
{
    m_linearVelocity.x = linearVelocityX;
    m_dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::SetLinearVelocityY(F32 linearVelocityY)
{
    m_linearVelocity.y = linearVelocityY;
    m_dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
F32 InputComponent::GetLinearVelocityX() const
{
    return m_linearVelocity.x;
}

//----------------------------------------------------------------------------------------------------
F32 InputComponent::GetLinearVelocityY() const
{
    return m_linearVelocity.y;
}

//----------------------------------------------------------------------------------------------------
void InputComponent::SetAngularVelocity(F32 angularVelocity)
{
    m_angularVelocity = angularVelocity;
    m_dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
F32 InputComponent::GetAngularVelocity() const
{
    return m_angularVelocity;
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Shoot()
{
    m_dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
}
}
