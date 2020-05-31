#include "HealthComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U64 HealthComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HEALTH)) {
        outputStream.Write(m_currentHealth);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HEALTH);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_MAX_HEALTH)) {
        outputStream.Write(m_maxHealth);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_MAX_HEALTH);
    }

    return writtenState;
}
}
