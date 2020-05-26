#ifndef __MOVEMENT_SYSTEM_H__
#define __MOVEMENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct RendererComponent;
struct TransformComponent;

//----------------------------------------------------------------------------------------------------
class MovementSystem : public System {

public:
    void DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, const glm::vec4& color) const;
};
}

#endif
