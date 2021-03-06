#ifndef __HEALTH_SYSTEM_SERVER_H__
#define __HEALTH_SYSTEM_SERVER_H__

#include "HealthSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HealthSystemServer : public HealthSystem, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::HEALTH;
    }

public:
    HealthSystemServer();

    bool PreUpdate() override;
    bool Update() override;
    bool Render() override;

    void OnNotify(const Event& event) override;

private:
    void OnWeaponHit(Entity shooterEntity, Entity targetEntity, U32 damage, const glm::vec2& direction) const;
};
}

#endif
