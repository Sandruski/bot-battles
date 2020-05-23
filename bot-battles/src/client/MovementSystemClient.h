#ifndef __MOVEMENT_SYSTEM_CLIENT_H__
#define __MOVEMENT_SYSTEM_CLIENT_H__

#include "MovementSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MovementSystemClient : public MovementSystem {
public:
    static SystemType GetType()
    {
        return SystemType::MOVEMENT;
    }

public:
    MovementSystemClient();

    bool Update() override;
    bool DebugRender() override;
};
}

#endif
