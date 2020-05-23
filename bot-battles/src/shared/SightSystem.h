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
    static SystemType GetType()
    {
        return SystemType::SIGHT;
    }

public:
    void DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<SightComponent> sightComponent) const;
};
}

#endif
