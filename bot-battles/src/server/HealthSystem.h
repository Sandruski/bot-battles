#ifndef __HEALTH_SYSTEM_H__
#define __HEALTH_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSystem : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::HEALTH;
    }

public:
    HealthSystem();

    bool PostUpdate() override;
};
}

#endif
