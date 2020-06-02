#ifndef __WEAPON_COMPONENT_H__
#define __WEAPON_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct WeaponComponent : public Component
#ifdef _CLIENT
    ,
                         public NetworkableReadObject
#elif defined(_SERVER)
    ,
                         public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::WEAPON; }

    WeaponComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    bool CanShootPrimary() const;
    void ShootPrimary();
    bool CanShootSecondary() const;
    void ShootSecondary();
    bool CanReload() const;
    void Reload();

    // Networked
    U32 m_damagePrimary;
    U32 m_damageSecondary;
    I32 m_currentAmmoPrimary;
    U32 m_maxAmmoPrimary;
    U32 m_ammoPrimary;
    F32 m_rangePrimary;
    F32 m_rangeSecondary;
    F32 m_timeShootPrimary;
    F32 m_cooldownShootPrimary;
    F32 m_timeShootSecondary;
    F32 m_cooldownShootSecondary;
    F32 m_timeReload;
    F32 m_cooldownReload;

    glm::vec2 m_originLastShot;
    glm::vec2 m_destinationLastShot;
    bool m_hasHitLastShot;

    // Local (server)
    bool m_isPickedUp;
};
}

#endif
