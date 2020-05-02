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

    bool PostUpdate() override;
    bool Render() override;
};
}

#endif
