#include "TransformComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U64 TransformComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION)) {
        outputStream.Write(m_position);
        writtenState |= static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_LAYER_TYPE)) {
        outputStream.Write(m_layerType);
        writtenState |= static_cast<U64>(ComponentMemberType::TRANSFORM_LAYER_TYPE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION)) {
        outputStream.Write(m_rotation);
        writtenState |= static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_SCALE)) {
        outputStream.Write(m_scale);
        writtenState |= static_cast<U64>(ComponentMemberType::TRANSFORM_SCALE);
    }

    return writtenState;
}
}
