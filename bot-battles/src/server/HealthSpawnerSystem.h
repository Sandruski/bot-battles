#ifndef __HEALTH_SPAWNER_SYSTEM_H__
#define __HEALTH_SPAWNER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSpawnerSystem : public System, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::HEALTH_SPAWNER;
    }

public:
    HealthSpawnerSystem();

    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    Entity SpawnHealth(U32 health, Entity spawner) const;

    void OnEntityRemoved(Entity entityRemoved) const;
};
}

#endif
