#ifndef __HEALTH_SYSTEM_H__
#define __HEALTH_SYSTEM_H__

#include "System.h"

namespace sand {

struct RendererComponent;
struct TransformComponent;
struct HealthComponent;

//----------------------------------------------------------------------------------------------------
class HealthSystem : public System {

public:
    void Draw(RendererComponent& rendererComponent, PlayerID playerID, std::weak_ptr<HealthComponent> healthComponent, const glm::vec4& color, const glm::vec4& backgroundColor) const;
};
}

#endif
