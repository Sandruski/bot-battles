#ifndef __REMOTE_PLAYER_WEAPON_SYSTEM_H__
#define __REMOTE_PLAYER_WEAPON_SYSTEM_H__

#include "WeaponSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RemotePlayerWeaponSystem : public WeaponSystem, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::REMOTE_PLAYER_WEAPON;
    }

public:
    RemotePlayerWeaponSystem();

    bool Render() override;
    bool RenderGui() override;
};
}

#endif
