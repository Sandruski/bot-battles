#include "TransformComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U32 TransformComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION)) {
        outputStream.Write(m_position);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_LAYER_TYPE)) {
        outputStream.Write(m_layerType);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_LAYER_TYPE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION)) {
        outputStream.Write(m_rotation);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_SCALE)) {
        outputStream.Write(m_scale);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_SCALE);
    }

    return writtenState;
}
}
