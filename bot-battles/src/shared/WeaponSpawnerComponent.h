#ifndef __WEAPON_SPAWNER_COMPONENT_H__
#define __WEAPON_SPAWNER_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct WeaponSpawnerComponent : public Component
#ifdef _CLIENT
    ,
                                public NetworkableReadObject
#elif defined(_SERVER)
    ,
                                public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::SPAWNER; }

    WeaponSpawnerComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    // Networked
    U32 m_weapon;
    U32 m_damageWeapon1;
    U32 m_damageWeapon2;
    U32 m_ammoWeapon1;
    U32 m_ammoWeapon2;
    F32 m_rangeWeapon1;
    F32 m_rangeWeapon2;
    F32 m_cooldownWeapon1;
    F32 m_cooldownWeapon2;

    // Local (server)
    MyTimer m_timer;
    F32 m_timeout;
    bool m_spawnWeapon1;
    bool m_spawnWeapon2;
};
}

#endif
