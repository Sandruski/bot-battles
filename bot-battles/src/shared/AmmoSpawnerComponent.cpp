#include "AmmoSpawnerComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
AmmoSpawnerComponent::AmmoSpawnerComponent()
    : m_ammo(0)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void AmmoSpawnerComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::AMMO_SPAWNER_AMMO)) {
        inputStream.Read(m_ammo);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 AmmoSpawnerComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::AMMO_SPAWNER_AMMO)) {
        outputStream.Write(m_ammo);
        writtenState |= static_cast<U32>(ComponentMemberType::AMMO_SPAWNER_AMMO);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
U32 AmmoSpawnerComponent::PickUp()
{
    U32 ammo = m_ammo;
    m_ammo = 0;

    return ammo;
}
}
