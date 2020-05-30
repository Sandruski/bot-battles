#ifndef __BOT_SPAWNER_SYSTEM_H__
#define __BOT_SPAWNER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class BotSpawnerSystem : public System, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::BOT_SPAWNER;
    }

public:
    BotSpawnerSystem();

    void OnNotify(const Event& event) override;

private:
    Entity SpawnBot(PlayerID playerID) const;
    void DespawnBot(Entity entity) const;

    void OnPlayerAdded(PlayerID playerID) const;
    void OnPlayerRemoved(Entity entity) const;
};
}

#endif
