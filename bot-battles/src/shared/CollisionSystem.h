#ifndef __COLLISION_SYSTEM_H__
#define __COLLISION_SYSTEM_H__

#include "System.h"

namespace sand {

struct TransformComponent;
struct ColliderComponent;

//----------------------------------------------------------------------------------------------------
class CollisionSystem : public System {

public:
    void DebugDraw(std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<ColliderComponent> colliderComponent) const;
};
}

#endif
