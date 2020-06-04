#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponComponent::WeaponComponent()
    : m_damagePrimary(0)
    , m_damageSecondary(0)
    , m_currentAmmoPrimary(0)
    , m_maxAmmoPrimary(0)
    , m_ammoPrimary(0)
    , m_rangePrimary(0.0f)
    , m_rangeSecondary(0.0f)
    , m_timeShootPrimary(0.0f)
    , m_cooldownShootPrimary(0.0f)
    , m_timeShootSecondary(0.0f)
    , m_cooldownShootSecondary(0.0f)
    , m_timeReload(0.0f)
    , m_cooldownReload(0.0f)
    , m_originLastShot(0.0f, 0.0f)
    , m_destinationLastShot(0.0f, 0.0f)
    , m_hasHitLastShot(false)
    , m_isPickedUp(false)
{
}

//----------------------------------------------------------------------------------------------------
bool WeaponComponent::CanShootPrimary() const
{
    return m_currentAmmoPrimary > 0;
}

//----------------------------------------------------------------------------------------------------
void WeaponComponent::ShootPrimary()
{
    m_currentAmmoPrimary -= 1;
}

//----------------------------------------------------------------------------------------------------
bool WeaponComponent::CanShootSecondary() const
{
    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponComponent::ShootSecondary()
{
}

//----------------------------------------------------------------------------------------------------
bool WeaponComponent::CanReload() const
{
    return m_ammoPrimary > 0;
}

//----------------------------------------------------------------------------------------------------
void WeaponComponent::Reload()
{
    I32 emptyAmmoPrimary = static_cast<I32>(m_maxAmmoPrimary) - m_currentAmmoPrimary;
    if (static_cast<I32>(m_ammoPrimary) >= emptyAmmoPrimary) {
        m_currentAmmoPrimary += emptyAmmoPrimary;
        m_ammoPrimary -= emptyAmmoPrimary;
    } else {
        m_currentAmmoPrimary += m_ammoPrimary;
        m_ammoPrimary = 0;
    }
}
}
