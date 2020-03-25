#include "SpawnComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SpawnComponent::SpawnComponent()
    : m_entity(INVALID_ENTITY)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SpawnComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 SpawnComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    return U32();
}
#endif
}
