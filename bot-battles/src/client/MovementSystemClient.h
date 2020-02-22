#ifndef __MOVEMENT_SYSTEM_CLIENT_H__
#define __MOVEMENT_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MovementSystemClient : public System {
public:
    static SystemType GetType()
    {
        return SystemType::MOVEMENT;
    }

public:
    MovementSystemClient();

    bool Update() override;
};
}

#endif
