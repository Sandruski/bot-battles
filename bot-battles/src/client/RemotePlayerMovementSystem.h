#ifndef __REMOTE_PLAYER_MOVEMENT_SYSTEM_H__
#define __REMOTE_PLAYER_MOVEMENT_SYSTEM_H__

#include "MovementSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RemotePlayerMovementSystem : public MovementSystem, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::REMOTE_PLAYER_MOVEMENT;
    }

public:
    RemotePlayerMovementSystem();

    bool Update() override;
    bool DebugRender() override;
};
}

#endif
