#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponComponent::WeaponComponent()
    : m_weaponPrimary(0)
    , m_damagePrimary(0)
    , m_damageSecondary(0)
    , m_ammoPrimary(0)
    , m_rangePrimary(0.0f)
    , m_rangeSecondary(0.0f)
    , m_cooldownPrimary(0.0f)
    , m_cooldownSecondary(0.0f)
    , m_hasShot(false)
    , m_hasHit(false)
    , m_weaponShot(0)
    , m_timestampShot(0.0f)
    , m_originShot(0.0f, 0.0f)
    , m_destinationShot(0.0f, 0.0f)
    , m_timeoutShot(1.0f)
    , m_timerShot()
{
}
}
