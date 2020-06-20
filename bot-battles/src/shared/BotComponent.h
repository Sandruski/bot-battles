#ifndef __BOT_COMPONENT_H__
#define __BOT_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct BotComponent : public Component
#ifdef _CLIENT
    ,
                      public NetworkableReadObject
#elif defined(_SERVER)
    ,
                      public NetworkableWriteObject
#endif
{
    enum class ActionType : U16 {

        NONE,
        SHOOT_PRIMARY,
        COOLDOWN_SHOOT_PRIMARY,
        SHOOT_SECONDARY,
        COOLDOWN_SHOOT_SECONDARY,
        RELOAD,
        COOLDOWN_RELOAD,
        HEAL,
        COOLDOWN_HEAL,
        KILL,
        DIE
    };

    static ComponentType GetType() { return ComponentType::BOT; }

    BotComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    bool CanPerformAction() const;

#ifdef _CLIENT
    // Python
    bool CanPyPerformAction() const
    {
        return CanPerformAction();
    }
#endif

    // Networked
    ActionType m_actionType;
    F32 m_timeAction;

#ifdef _SERVER
    // Local (server)
    F32 m_cooldownAction;
    MyTimer m_timerAction;
#endif
};
}
#endif
