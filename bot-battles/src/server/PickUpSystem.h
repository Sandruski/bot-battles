#ifndef __PICK_UP_SYSTEM_H__
#define __PICK_UP_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class PickUpSystem : public System, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::PICK_UP;
    }

public:
    PickUpSystem();

    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    void OnCollisionEnter(Entity entityA, Entity entityB) const;
};
}

#endif
