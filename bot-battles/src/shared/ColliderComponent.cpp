#include "ColliderComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ColliderComponent::ColliderComponent()
    : m_center()
    , m_size()
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

//----------------------------------------------------------------------------------------------------
bool ColliderComponent::IsColliding(const ColliderComponent& colliderComponent) const
{
    F32 x1A = m_center.x - m_size.x * 0.5f;
    F32 x2A = m_center.x + m_size.x * 0.5f;
    F32 y1A = m_center.y - m_size.y * 0.5f;
    F32 y2A = m_center.y + m_size.y * 0.5f;

    F32 x1B = colliderComponent.m_center.x - colliderComponent.m_size.x * 0.5f;
    F32 x2B = colliderComponent.m_center.x + colliderComponent.m_size.x * 0.5f;
    F32 y1B = colliderComponent.m_center.y - colliderComponent.m_size.y * 0.5f;
    F32 y2B = colliderComponent.m_center.y + colliderComponent.m_size.y * 0.5f;

    return (x1A <= x2B && x2A >= x1B && y1A <= y2B && y2A >= y1B);
}
}
