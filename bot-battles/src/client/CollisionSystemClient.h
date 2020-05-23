#ifndef __COLLISION_SYSTEM_CLIENT_H__
#define __COLLISION_SYSTEM_CLIENT_H__

#include "CollisionSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class CollisionSystemClient : public CollisionSystem {
public:
    static SystemType GetType()
    {
        return SystemType::COLLISION;
    }

public:
    CollisionSystemClient();

    bool DebugRender() override;
};
}

#endif
