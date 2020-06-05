#ifndef __HEALTH_COMPONENT_H__
#define __HEALTH_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct HealthComponent : public Component
#ifdef _CLIENT
    ,
                         public NetworkableReadObject
#elif defined(_SERVER)
    ,
                         public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::HEALTH; }

    HealthComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    bool CanHeal() const;
    void Heal();

    // Bot
    U32 GetCurrentHP() const
    {
        return static_cast<U32>(m_currentHP);
    }

    U32 GetMaxHP() const
    {
        return m_maxHP;
    }

    bool HasFirstAidBox() const
    {
        return m_HP > 0;
    }

    U32 GetFirstAidBoxHP() const
    {
        return m_HP;
    }

    F32 GetHealingTime() const
    {
        return m_timeHeal;
    }

    F32 GetHealingCooldown() const
    {
        return m_cooldownHeal;
    }

    // Networked
    I32 m_currentHP;
    U32 m_maxHP;
    U32 m_HP;
    F32 m_timeHeal;
    F32 m_cooldownHeal;

    // Local (server)
    bool m_isPickedUp;
};
}

#endif
