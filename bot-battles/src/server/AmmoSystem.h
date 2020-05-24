#ifndef __AMMO_SYSTEM_H__
#define __AMMO_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class AmmoSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::AMMO;
    }

public:
    AmmoSystem();

    bool Update() override;
};
}

#endif
