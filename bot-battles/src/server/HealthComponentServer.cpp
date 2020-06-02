#include "HealthComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U64 HealthComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP)) {
        outputStream.Write(m_currentHP);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_MAX_HP)) {
        outputStream.Write(m_maxHP);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_MAX_HP);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_HP)) {
        outputStream.Write(m_HP);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_HP);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_TIME_HEAL)) {
        outputStream.Write(m_timeHeal);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_TIME_HEAL);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_COOLDOWN_HEAL)) {
        outputStream.Write(m_cooldownHeal);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_COOLDOWN_HEAL);
    }

    return writtenState;
}
}
