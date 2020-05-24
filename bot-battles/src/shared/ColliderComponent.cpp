#include "ColliderComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ColliderComponent::ColliderComponent()
    : m_size(0.0f, 0.0f)
    , m_shapeType(ShapeType::NONE)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void ColliderComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SIZE)) {
        inputStream.Read(m_size);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SHAPE_TYPE)) {
        inputStream.Read(m_shapeType);
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
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SHAPE_TYPE)) {
        outputStream.Write(m_shapeType);
        writtenState |= static_cast<U32>(ComponentMemberType::COLLIDER_SHAPE_TYPE);
    }

    return writtenState;
}
#endif
}
