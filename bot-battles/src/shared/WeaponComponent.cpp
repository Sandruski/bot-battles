#include "WeaponComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponComponent::WeaponComponent()
    : m_positions()
    , m_origin(0.0f, 0.0f)
    , m_destination(0.0f, 0.0f)
    , m_hasHit(false)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WeaponComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    // TODO
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 WeaponComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    // TODO
    return 0;
}
#endif
}
