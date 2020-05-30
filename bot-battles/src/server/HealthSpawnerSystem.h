#ifndef __HEALTH_SPAWNER_SYSTEM_H__
#define __HEALTH_SPAWNER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSpawnerSystem : public System, public Subject, public Observer {
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
    void DespawnHealth(Entity entity) const;
    bool PickUpHealth(Entity character, Entity health) const;

    void OnCollisionEnter(Entity entityA, Entity entityB) const;
    void OnEntityRemoved(Entity entityRemoved) const;
};
}

#endif
