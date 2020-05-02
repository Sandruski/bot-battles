#ifndef __REMOTE_PLAYER_MOVEMENT_SYSTEM_H__
#define __REMOTE_PLAYER_MOVEMENT_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RemotePlayerMovementSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::REMOTE_PLAYER_MOVEMENT;
    }

public:
    RemotePlayerMovementSystem();

    bool PreUpdate() override;
};
}

#endif
