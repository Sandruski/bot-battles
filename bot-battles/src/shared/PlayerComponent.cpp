#include "PlayerComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void PlayerComponent::Read(InputMemoryStream& /*inputStream*/, U64 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 PlayerComponent::Write(OutputMemoryStream& /*outputStream*/, U64 /*dirtyState*/) const
{
    U64 writtenState = 0;

    return writtenState;
}
#endif
}
