#ifndef __COLLISION_SYSTEM_H__
#define __COLLISION_SYSTEM_H__

#include "System.h"

namespace sand {

struct RendererComponent;
struct TransformComponent;
struct ColliderComponent;

//----------------------------------------------------------------------------------------------------
class CollisionSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::COLLISION;
    }

public:
    void DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<ColliderComponent> colliderComponent, const glm::vec4& color) const;
};
}

#endif
