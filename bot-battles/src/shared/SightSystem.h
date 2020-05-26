#ifndef __SIGHT_SYSTEM_H__
#define __SIGHT_SYSTEM_H__

#include "System.h"

namespace sand {

struct RendererComponent;
struct TransformComponent;
struct SightComponent;

//----------------------------------------------------------------------------------------------------
class SightSystem : public System {

public:
    void DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<SightComponent> sightComponent, const glm::vec4& color) const;
};
}

#endif
