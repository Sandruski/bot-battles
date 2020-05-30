#ifndef __WEAPON_SPAWNER_SYSTEM_H__
#define __WEAPON_SPAWNER_SYSTEM_H__

#include "System.h"

namespace sand {

struct WeaponSpawnerComponent;

//----------------------------------------------------------------------------------------------------
class WeaponSpawnerSystem : public System, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::WEAPON_SPAWNER;
    }

public:
    WeaponSpawnerSystem();

    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    Entity SpawnWeapon(U32 weapon, Entity spawner) const;

    void OnEntityRemoved(Entity entityRemoved) const;
};
}

#endif
