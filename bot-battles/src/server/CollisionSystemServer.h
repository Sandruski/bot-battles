#ifndef __COLLISION_SYSTEM_SERVER_H__
#define __COLLISION_SYSTEM_SERVER_H__

#include "CollisionSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class CollisionSystemServer : public CollisionSystem {
public:
    static SystemType GetType()
    {
        return SystemType::COLLISION;
    }

public:
    CollisionSystemServer();

    bool DebugRender() override;
};
}

#endif
