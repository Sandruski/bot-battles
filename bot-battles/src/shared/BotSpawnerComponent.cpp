#include "BotSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotSpawnerComponent::BotSpawnerComponent()
    : m_facing(0.0f)
    , m_sightAngle(0.0f)
    , m_sightDistance(0.0f)
    , m_playerID(INVALID_PLAYER_ID)
    , m_health(0)
    , m_damageWeapon(0)
    , m_rangeWeapon(0.0f)
    , m_cooldownWeapon(0.0f)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void BotSpawnerComponent::Read(InputMemoryStream& /*inputStream*/, U64 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 BotSpawnerComponent::Write(OutputMemoryStream& /*outputStream*/, U64 /*dirtyState*/) const
{
    U64 writtenState = 0;

    return writtenState;
}
#endif
}
