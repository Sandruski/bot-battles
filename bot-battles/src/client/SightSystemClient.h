#ifndef __SIGHT_SYSTEM_CLIENT_H__
#define __SIGHT_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

struct PhysicsComponent;

//----------------------------------------------------------------------------------------------------
class SightSystemClient : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::SIGHT;
    }

public:
    SightSystemClient();

    bool PreUpdate() override;
    bool Update() override;
    bool DebugRender() override;

private:
    bool IsInFoV(glm::vec2 position, glm::vec2 targetPosition, glm::vec2 direction, F32 angle) const;
    bool IsInLoS(PhysicsComponent& physicsComponent, glm::vec2 position, glm::vec2 targetPosition, F32 distance) const;
};
}

#endif
