#ifndef __HEALTH_SPAWNER_SYSTEM_H__
#define __HEALTH_SPAWNER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSpawnerSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::HEALTH_SPAWNER;
    }

public:
    HealthSpawnerSystem();

    bool Update() override;
};
}

#endif
