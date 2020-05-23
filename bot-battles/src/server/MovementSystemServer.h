#ifndef __MOVEMENT_SYSTEM_SERVER_H__
#define __MOVEMENT_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MovementSystemServer : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::MOVEMENT;
    }

public:
    MovementSystemServer();

    bool PreUpdate() override;
    bool Update() override;
    bool RenderGui() override;
};
}

#endif
