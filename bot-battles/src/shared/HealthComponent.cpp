#include "HealthComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthComponent::HealthComponent()
    : m_hitEntityLastShot(INVALID_ENTITY)
    , m_directionLastShot(0.0f, 0.0f)
    , m_currentHP(0)
    , m_maxHP(0)
    , m_HP(0)
    , m_timeHeal(0.0f)
    , m_cooldownHeal(0.0f)
#ifdef _SERVER
    , m_isPickedUp(false)
#endif
{
}

//----------------------------------------------------------------------------------------------------
bool HealthComponent::CanHeal() const
{
    return m_HP > 0;
}

//----------------------------------------------------------------------------------------------------
void HealthComponent::Heal()
{
    m_currentHP += m_HP;
    if (m_currentHP > static_cast<I32>(m_maxHP)) {
        m_currentHP = m_maxHP;
    }
    m_HP = 0;
}
}
