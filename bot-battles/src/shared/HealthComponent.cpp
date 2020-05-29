#include "HealthComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthComponent::HealthComponent()
    : m_currentHealth(0)
    , m_maxHealth(0)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HEALTH)) {
        inputStream.Read(m_currentHealth);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_MAX_HEALTH)) {
        inputStream.Read(m_maxHealth);
    }
}
#elif defined(_SERVER)
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
#endif
}
