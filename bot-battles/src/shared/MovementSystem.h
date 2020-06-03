#ifndef __MOVEMENT_SYSTEM_H__
#define __MOVEMENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct TransformComponent;

//----------------------------------------------------------------------------------------------------
class MovementSystem : public System {

public:
    void DebugDraw(std::weak_ptr<TransformComponent> transformComponent) const;
};
}

#endif
