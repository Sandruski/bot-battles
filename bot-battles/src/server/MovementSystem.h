#ifndef __MOVEMENT_SYSTEM_H__
#define __MOVEMENT_SYSTEM_H__

#include "ComponentDefs.h"
#include "System.h"

namespace sand {

class InputState;

//----------------------------------------------------------------------------------------------------
class MovementSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::MOVEMENT;
    }

public:
    MovementSystem();
    ~MovementSystem() override;

    bool StartUp() override;
    bool Update() override;
    bool ShutDown() override;

private:
    void ProcessInput(F32 dt, const InputState& inputState) const;
    void UpdateMovement(F32 dt) const;
};
}

#endif
