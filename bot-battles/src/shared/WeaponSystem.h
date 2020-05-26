#ifndef __WEAPON_SYSTEM_H__
#define __WEAPON_SYSTEM_H__

#include "System.h"

namespace sand {

struct RendererComponent;
struct WeaponComponent;

//----------------------------------------------------------------------------------------------------
class WeaponSystem : public System {

public:
    void DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<WeaponComponent> weaponComponent, const glm::vec4& color) const;
};
}

#endif
