#ifndef __HEALTH_SYSTEM_H__
#define __HEALTH_SYSTEM_H__

#include "System.h"

namespace sand {

struct HealthComponent;

//----------------------------------------------------------------------------------------------------
class HealthSystem : public System {

public:
    void Draw(PlayerID playerID, std::weak_ptr<HealthComponent> healthComponent) const;
};
}

#endif
