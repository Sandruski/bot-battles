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

    // Python
    U32 GetPrimaryWeaponDamage() const
    {
        return m_damagePrimary;
    }

    U32 GetSecondaryWeaponDamage() const
    {
        return m_damageSecondary;
    }

    U32 GetCurrentAmmo() const
    {
        return static_cast<U32>(m_currentAmmoPrimary);
    }

    U32 GetMaxAmmo() const
    {
        return static_cast<U32>(m_maxAmmoPrimary);
    }

    bool HasAmmoBox() const
    {
        return m_ammoPrimary > 0;
    }

    U32 GetAmmoBoxAmmo() const
    {
        return m_ammoPrimary;
    }

    F32 GetPrimaryWeaponRange() const
    {
        return m_rangePrimary;
    }

    F32 GetSecondaryWeaponRange() const
    {
        return m_rangeSecondary;
    }

    F32 GetPrimaryWeaponShootingTime() const
    {
        return m_timeShootPrimary;
    }

    F32 GetPrimaryWeaponShootingCooldown() const
    {
        return m_cooldownShootPrimary;
    }

    F32 GetSecondaryWeaponShootingTime() const
    {
        return m_timeShootSecondary;
    }

    F32 GetSecondaryWeaponShootingCooldown() const
    {
        return m_cooldownShootSecondary;
    }

    F32 GetReloadingTime() const
    {
        return m_timeReload;
    }

    F32 GetReloadingCooldown() const
    {
        return m_cooldownReload;
    }

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
