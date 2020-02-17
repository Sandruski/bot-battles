#ifndef __WEAPON_SYSTEM_SERVER_H__
#define __WEAPON_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WeaponSystemServer : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON;
    }

public:
    WeaponSystemServer();

    bool Update() override;
    bool DebugRender() override;

private:
    void Rewind(Entity localEntity, U32 from, U32 to, F32 percentage);
    void Revert(Entity localEntity);
};
}

#endif
