#include "HealthSpawnerComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSpawnerComponent::HealthSpawnerComponent()
    : m_hasHealth(false)
    , m_health(0)
    , m_timer()
    , m_timeout(0)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void HealthSpawnerComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_SPAWNER_HAS_HEALTH)) {
        inputStream.Read(m_hasHealth);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_SPAWNER_HEALTH)) {
        inputStream.Read(m_health);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 HealthSpawnerComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_SPAWNER_HAS_HEALTH)) {
        outputStream.Write(m_hasHealth);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_SPAWNER_HAS_HEALTH);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_SPAWNER_HEALTH)) {
        outputStream.Write(m_health);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_SPAWNER_HEALTH);
    }

    return writtenState;
}
#endif
}
