#ifndef __COLLISION_SYSTEM_CLIENT_H__
#define __COLLISION_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class CollisionSystemClient : public System {
public:
    static SystemType GetType()
    {
        return SystemType::COLLISION;
    }

public:
    CollisionSystemClient();

    bool Update() override;
    bool DebugRender() override;
};
}

#endif
