#include "WallComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WallComponent::WallComponent()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WallComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 WallComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    U32 writtenState = 0;

    return writtenState;
}
#endif
}
