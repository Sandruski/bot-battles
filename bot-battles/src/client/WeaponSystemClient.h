#ifndef __WEAPON_SYSTEM_CLIENT_H__
#define __WEAPON_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WeaponSystemClient : public System {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON;
    }

public:
    WeaponSystemClient();

    bool Update() override;
    bool DebugRender() override;
};
}

#endif
