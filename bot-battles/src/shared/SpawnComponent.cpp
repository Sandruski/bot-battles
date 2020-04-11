#include "SpawnComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SpawnComponent::SpawnComponent()
    : m_entity(INVALID_ENTITY)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SpawnComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPAWN_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 SpawnComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::SPAWN_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::SPAWN_ENABLED);
    }

    return writtenState;
}
#endif
}
