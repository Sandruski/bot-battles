#include "ColliderComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ColliderComponent::ColliderComponent()
    : m_size(0.0f, 0.0f)
    , m_shapeType(ShapeType::NONE)
    , m_isTrigger(false)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void ColliderComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::COLLIDER_SIZE)) {
        inputStream.Read(m_size);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::COLLIDER_SHAPE_TYPE)) {
        inputStream.Read(m_shapeType);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::COLLIDER_TRIGGER)) {
        inputStream.Read(m_isTrigger);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 ColliderComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::COLLIDER_SIZE)) {
        outputStream.Write(m_size);
        writtenState |= static_cast<U64>(ComponentMemberType::COLLIDER_SIZE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::COLLIDER_SHAPE_TYPE)) {
        outputStream.Write(m_shapeType);
        writtenState |= static_cast<U64>(ComponentMemberType::COLLIDER_SHAPE_TYPE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::COLLIDER_TRIGGER)) {
        outputStream.Write(m_isTrigger);
        writtenState |= static_cast<U64>(ComponentMemberType::COLLIDER_TRIGGER);
    }

    return writtenState;
}
#endif
}
