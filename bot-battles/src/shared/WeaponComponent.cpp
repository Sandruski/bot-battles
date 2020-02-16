#include "WeaponComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponComponent::WeaponComponent()
    : m_origin()
    , m_destination()
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

//----------------------------------------------------------------------------------------------------
SDL_Rect WeaponComponent::GetShotRect() const
{
    I32 x1 = static_cast<I32>(m_origin.x);
    I32 y1 = static_cast<I32>(m_origin.y);
    I32 x2 = static_cast<I32>(m_destination.x);
    I32 y2 = static_cast<I32>(m_destination.y);

    SDL_Rect rect = { x1, y1, x2, y2 };

    return rect;
}
}
