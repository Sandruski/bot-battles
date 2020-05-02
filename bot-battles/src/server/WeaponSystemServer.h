#ifndef __WEAPON_SYSTEM_SERVER_H__
#define __WEAPON_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

struct WeaponComponent;

//----------------------------------------------------------------------------------------------------
class WeaponSystemServer : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON;
    }

public:
    WeaponSystemServer();

    bool PostUpdate() override;
    bool Render() override;

private:
    void Rewind(std::weak_ptr<WeaponComponent> weaponComponent, Entity localEntity, U32 from, U32 to, F32 percentage);
    void Revert(Entity localEntity);
};
}

#endif
