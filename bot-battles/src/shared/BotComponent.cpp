#include "BotComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotComponent::BotComponent()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void BotComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 BotComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    U32 writtenState = 0;

    return writtenState;
}
#endif
}
