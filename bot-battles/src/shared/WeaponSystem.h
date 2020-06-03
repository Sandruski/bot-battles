#ifndef __WEAPON_SYSTEM_H__
#define __WEAPON_SYSTEM_H__

#include "System.h"

namespace sand {

struct WeaponComponent;

//----------------------------------------------------------------------------------------------------
class WeaponSystem : public System {

public:
    void Draw(PlayerID playerID, std::weak_ptr<WeaponComponent> weaponComponent) const;
    void DrawGui(PlayerID playerID, std::weak_ptr<WeaponComponent> weaponComponent) const;
};
}

#endif
