#ifndef __WEAPON_SYSTEM_H__
#define __WEAPON_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WeaponSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON;
    }

public:
    WeaponSystem();

    bool Update() override;
    bool DebugRender() override;

private: // TODO: remove this private variables
    // Debug raycast
    SDL_Rect line;
    SDL_Color color;
    bool shoot = false;

    // Debug server rewind
    SDL_Rect befColl;
    SDL_Rect aftColl;
};
}

#endif
