#include "SightComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightComponent::SightComponent()
    : m_angle(0.0f)
    , m_distance(0.0f)
#ifdef _CLIENT
    , m_target(INVALID_ENTITY)
#endif
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SightComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SIGHT_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SIGHT_ANGLE)) {
        inputStream.Read(m_angle);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SIGHT_DISTANCE)) {
        inputStream.Read(m_distance);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 SightComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::SIGHT_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::SIGHT_ENABLED);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SIGHT_ANGLE)) {
        outputStream.Write(m_angle);
        writtenState |= static_cast<U32>(ComponentMemberType::SIGHT_ANGLE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SIGHT_DISTANCE)) {
        outputStream.Write(m_distance);
        writtenState |= static_cast<U32>(ComponentMemberType::SIGHT_DISTANCE);
    }

    return writtenState;
}
#endif
}
