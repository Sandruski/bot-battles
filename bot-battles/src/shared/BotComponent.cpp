#include "BotComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotComponent::BotComponent()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void BotComponent::Read(InputMemoryStream& /*inputStream*/, U64 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 BotComponent::Write(OutputMemoryStream& /*outputStream*/, U64 /*dirtyState*/) const
{
    U64 writtenState = 0;

    return writtenState;
}
#endif
}
