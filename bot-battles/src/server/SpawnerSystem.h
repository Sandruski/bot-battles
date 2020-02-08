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
    void OnNotify(const Event& event) override;

private:
    Entity SpawnPlayerEntity(U32 number, const std::string& name) const;

    void OnPlayerAdded(PlayerID playerID) const;
    void OnPlayerRemoved(Entity entity) const;
};
}

#endif
