#ifndef __COLLISION_SYSTEM_SERVER_H__
#define __COLLISION_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class CollisionSystemServer : public System {
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
