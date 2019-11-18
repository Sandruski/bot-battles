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
    ~SpawnerSystem() override;

    void OnNotify(const Event& event) override;

private:
    Entity SpawnPlayerEntity() const;
};
}

#endif