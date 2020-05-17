#ifndef __HEALTH_SYSTEM_H__
#define __HEALTH_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSystem : public System, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::HEALTH;
    }

public:
    HealthSystem();

    bool PreUpdate() override;
    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    void OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 normal) const;
    void OnWeaponHit(Entity entity, U32 damage) const;
};
}

#endif
