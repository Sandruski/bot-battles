#include "InputComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
InputComponent::InputComponent()
    : m_maxLinearVelocity(300.0f)
    , m_maxAngularVelocity(120.0f)
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
void InputComponent::SetLinearVelocity(const glm::vec2& linearVelocity)
{
    m_linearVelocity = linearVelocity;
    m_dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
glm::vec2 InputComponent::GetLinearVelocity() const
{
    return m_linearVelocity;
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
