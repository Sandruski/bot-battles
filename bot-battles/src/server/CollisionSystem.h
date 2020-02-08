#ifndef __COLLISION_SYSTEM_H__
#define __COLLISION_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class CollisionSystem : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::COLLISION;
    }

public:
    CollisionSystem();

    bool Update() override;

    bool DebugRender() override;
};
}

#endif
