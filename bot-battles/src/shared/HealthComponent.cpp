#include "HealthComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthComponent::HealthComponent()
    : m_health(0)
    , m_isDead(false)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::HEALTH_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::HEALTH_HEALTH)) {
        inputStream.Read(m_health);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::HEALTH_DEAD)) {
        inputStream.Read(m_isDead);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 HealthComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::HEALTH_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::HEALTH_ENABLED);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::HEALTH_HEALTH)) {
        outputStream.Write(m_health);
        writtenState |= static_cast<U32>(ComponentMemberType::HEALTH_HEALTH);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::HEALTH_DEAD)) {
        outputStream.Write(m_isDead);
        writtenState |= static_cast<U32>(ComponentMemberType::HEALTH_DEAD);
    }

    return writtenState;
}
#endif
}
