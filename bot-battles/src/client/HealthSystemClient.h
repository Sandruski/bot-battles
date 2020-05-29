#ifndef __HEALTH_SYSTEM_CLIENT_H__
#define __HEALTH_SYSTEM_CLIENT_H__

#include "HealthSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSystemClient : public HealthSystem {
public:
    static SystemType GetType()
    {
        return SystemType::HEALTH;
    }

public:
    HealthSystemClient();

    bool Render() override;
};
}

#endif
