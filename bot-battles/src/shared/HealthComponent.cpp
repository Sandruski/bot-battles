#include "HealthComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthComponent::HealthComponent()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    // TODO
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 HealthComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    // TODO
    return 0;
}
#endif
}
