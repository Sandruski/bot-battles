#include "InputComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
InputComponent::InputComponent()
    : m_linearVelocity(0.0f, 0.0f)
    , m_angularVelocity(0.0f)
    , m_dirtyState(0)
{
}

//----------------------------------------------------------------------------------------------------
U64 InputComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY)) {
        outputStream.Write(m_linearVelocity);
        writtenState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
    }
    if (dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY)) {
        outputStream.Write(m_angularVelocity);
        writtenState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
    }

    return writtenState;
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY)) {
        inputStream.Read(m_linearVelocity);
    }
    if (dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY)) {
        inputStream.Read(m_angularVelocity);
    }
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Reset()
{
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_HEAL);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::FullReset()
{
    m_dirtyState = 0;
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void InputComponent::SetLinearVelocityX(F32 linearVelocityX)
{
    m_linearVelocity.x = linearVelocityX;
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::SetLinearVelocityY(F32 linearVelocityY)
{
    m_linearVelocity.y = linearVelocityY;
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::SetAngularVelocity(F32 angularVelocity)
{
    m_angularVelocity = angularVelocity;
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::ShootPrimaryWeapon()
{
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);

    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_HEAL);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::ShootSecondaryWeapon()
{
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);

    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_HEAL);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Reload()
{
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);

    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_HEAL);
}

//----------------------------------------------------------------------------------------------------
void InputComponent::Heal()
{
    m_dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_HEAL);

    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
    m_dirtyState &= ~static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);
}
#endif
}
