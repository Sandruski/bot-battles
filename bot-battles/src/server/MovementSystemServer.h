#ifndef __MOVEMENT_SYSTEM_SERVER_H__
#define __MOVEMENT_SYSTEM_SERVER_H__

#include "MovementSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MovementSystemServer : public MovementSystem, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::MOVEMENT;
    }

public:
    MovementSystemServer();

    bool PreUpdate() override;
    bool Update() override;
    bool DebugRender() override;
};
}

#endif
