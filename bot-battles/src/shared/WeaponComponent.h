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

    // Networked
    U32 m_damagePrimary;
    U32 m_damageSecondary;
    U32 m_ammoPrimary;
    F32 m_rangePrimary;
    F32 m_rangeSecondary;
    F32 m_cooldownPrimary;
    F32 m_cooldownSecondary;

    // Local
    F32 m_timestampLastShotPrimary;
    F32 m_timestampLastShotSecondary;

    glm::vec2 m_originShot;
    glm::vec2 m_destinationShot;
    F32 m_timeoutShot;
    MyTimer m_timerShot;
};
}

#endif
