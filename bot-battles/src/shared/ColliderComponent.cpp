#include "ColliderComponent.h"

#include "ComponentMemberTypes.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ColliderComponent::ColliderComponent()
    : m_position()
    , m_size()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void ColliderComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SIZE)) {
        inputStream.Read(m_size);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 ColliderComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SIZE)) {
        outputStream.Write(m_size);
        writtenState |= static_cast<U32>(ComponentMemberType::COLLIDER_SIZE);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
SDL_Rect ColliderComponent::GetRect() const
{
    I32 x1 = static_cast<I32>(m_position.x - m_size.x / 2.0f);
    I32 y1 = static_cast<I32>(m_position.y - m_size.y / 2.0f);
    I32 x2 = static_cast<I32>(m_size.x);
    I32 y2 = static_cast<I32>(m_size.y);

    SDL_Rect rect = { x1, y1, x2, y2 };

    return rect;
}
}
