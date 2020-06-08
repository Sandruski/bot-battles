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

#ifdef _CLIENT
    // Python
    U32 GetPyPrimaryWeaponDamage() const
    {
        return m_damagePrimary;
    }

    U32 GetPySecondaryWeaponDamage() const
    {
        return m_damageSecondary;
    }

    U32 GetPyCurrentAmmo() const
    {
        return static_cast<U32>(m_currentAmmoPrimary);
    }

    U32 GetPyMaxAmmo() const
    {
        return static_cast<U32>(m_maxAmmoPrimary);
    }

    U32 GetPyAmmoBoxAmmo() const
    {
        return m_ammoPrimary;
    }

    F32 GetPyPrimaryWeaponRange() const
    {
        return m_rangePrimary;
    }

    F32 GetPySecondaryWeaponRange() const
    {
        return m_rangeSecondary;
    }

    F32 GetPyPrimaryWeaponShootingTime() const
    {
        return m_timeShootPrimary;
    }

    F32 GetPyPrimaryWeaponShootingCooldown() const
    {
        return m_cooldownShootPrimary;
    }

    F32 GetPySecondaryWeaponShootingTime() const
    {
        return m_timeShootSecondary;
    }

    F32 GetPySecondaryWeaponShootingCooldown() const
    {
        return m_cooldownShootSecondary;
    }

    F32 GetPyReloadingTime() const
    {
        return m_timeReload;
    }

    F32 GetPyReloadingCooldown() const
    {
        return m_cooldownReload;
    }
#endif

    // Networked
    glm::vec2 m_originLastShot;
    glm::vec2 m_destinationLastShot;
    Entity m_hitEntityLastShot;

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

    // Local (server)
    bool m_isPickedUp;
};
}

#endif
