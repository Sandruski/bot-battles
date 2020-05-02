#ifndef __PHYSICS_SYSTEM_H__
#define __PHYSICS_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class PhysicsSystem : public System, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::PHYSICS;
    }

public:
    PhysicsSystem();

    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    void OnRigidbodyComponentAdded(Entity entity) const;
};
}

#endif
