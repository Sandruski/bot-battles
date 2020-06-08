#include "SightComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightComponent::SightComponent()
    : m_angle(0.0f)
    , m_distance(0.0f)
    , m_seenEntities()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SightComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::SIGHT_ANGLE)) {
        inputStream.Read(m_angle);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SIGHT_DISTANCE)) {
        inputStream.Read(m_distance);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 SightComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::SIGHT_ANGLE)) {
        outputStream.Write(m_angle);
        writtenState |= static_cast<U64>(ComponentMemberType::SIGHT_ANGLE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SIGHT_DISTANCE)) {
        outputStream.Write(m_distance);
        writtenState |= static_cast<U64>(ComponentMemberType::SIGHT_DISTANCE);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
bool SightComponent::IsEntitySeen(Entity entity)
{
    return std::find(m_seenEntities.begin(), m_seenEntities.end(), entity) != m_seenEntities.end();
}
}
