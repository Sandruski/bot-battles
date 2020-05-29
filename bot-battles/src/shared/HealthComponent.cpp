#include "HealthComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthComponent::HealthComponent()
    : m_health(0)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_HEALTH)) {
        inputStream.Read(m_health);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 HealthComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_HEALTH)) {
        outputStream.Write(m_health);
        writtenState |= static_cast<U64>(ComponentMemberType::HEALTH_HEALTH);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
bool HealthComponent::IsAlive() const
{
    return m_health > 0;
}
}
