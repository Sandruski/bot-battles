#include "WallComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WallComponent::WallComponent()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WallComponent::Read(InputMemoryStream& /*inputStream*/, U64 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 WallComponent::Write(OutputMemoryStream& /*outputStream*/, U64 /*dirtyState*/) const
{
    U64 writtenState = 0;

    return writtenState;
}
#endif
}
