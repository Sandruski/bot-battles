#include "AmmoSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
AmmoSpawnerComponent::AmmoSpawnerComponent()
    : m_ammo(0)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void AmmoSpawnerComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 AmmoSpawnerComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    U32 writtenState = 0;

    return writtenState;
}
#endif
}
