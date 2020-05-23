#ifndef __SIGHT_SYSTEM_SERVER_H__
#define __SIGHT_SYSTEM_SERVER_H__

#include "SightSystem.h"

namespace sand {

struct PhysicsComponent;

//----------------------------------------------------------------------------------------------------
class SightSystemServer : public SightSystem, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::SIGHT;
    }

public:
    SightSystemServer();

    bool PreUpdate() override;
    bool Update() override;
    bool DebugRender() override;

private:
    bool IsInFoV(glm::vec2 position, glm::vec2 targetPosition, glm::vec2 direction, F32 angle) const;
    bool IsInLoS(PhysicsComponent& physicsComponent, glm::vec2 position, glm::vec2 targetPosition, F32 distance, Entity entity) const;
};
}

#endif
