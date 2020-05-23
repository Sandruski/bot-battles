#ifndef __WEAPON_SYSTEM_SERVER_H__
#define __WEAPON_SYSTEM_SERVER_H__

#include "WeaponSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WeaponSystemServer : public WeaponSystem, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON;
    }

public:
    WeaponSystemServer();

    bool PreUpdate() override;
    bool Update() override;
    bool Render() override;

private:
    void Rewind(Entity localEntity, U32 from, U32 to, F32 percentage) const;
    void Revert(Entity localEntity) const;
};
}

#endif
