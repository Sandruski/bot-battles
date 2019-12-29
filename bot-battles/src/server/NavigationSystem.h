#ifndef __NAVIGATION_SYSTEM_H__
#define __NAVIGATION_SYSTEM_H__

#include "System.h"

namespace sand {

struct InputComponent;
struct TransformComponent;

//----------------------------------------------------------------------------------------------------
class NavigationSystem : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::NAVIGATION;
    }

public:
    NavigationSystem();

    bool Update() override;

private:
    void UpdateMovement(Entity entity, const InputComponent& input, TransformComponent& transform, F32 dt) const;
};
}

#endif
