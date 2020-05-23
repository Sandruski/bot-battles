#ifndef __WEAPON_SYSTEM_CLIENT_H__
#define __WEAPON_SYSTEM_CLIENT_H__

#include "WeaponSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WeaponSystemClient : public WeaponSystem {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON;
    }

public:
    WeaponSystemClient();

    bool Update() override;
    bool Render() override;
};
}

#endif
