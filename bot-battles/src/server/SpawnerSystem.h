#ifndef __SPAWNER_SYSTEM_H__
#define __SPAWNER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SpawnerSystem : public System, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::SPAWNER;
    }

public:
    SpawnerSystem();

    void OnNotify(const Event& event) override;

private:
    Entity Spawn(U32 number) const;
    void Despawn(Entity entity) const;

    void OnPlayerAdded(PlayerID playerID) const;
    void OnPlayerRemoved(Entity entity) const;
};
}

#endif
