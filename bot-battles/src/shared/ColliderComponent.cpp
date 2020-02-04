#include "ColliderComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ColliderComponent::ColliderComponent()
    : m_size()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void ColliderComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    // TODO
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 ColliderComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    // TODO
    return 0;
}
#endif
}
