#ifndef __NAVIGATION_SYSTEM_H__
#define __NAVIGATION_SYSTEM_H__

#include "ComponentDefs.h"
#include "System.h"

namespace sand {

class InputState;

//----------------------------------------------------------------------------------------------------
class NavigationSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::NAVIGATION;
    }

public:
    NavigationSystem();
    ~NavigationSystem() override;

    bool StartUp() override;
    bool Update() override;
    bool ShutDown() override;

private:
    void ProcessInput(F32 dt, const InputState& inputState) const;
    void UpdateMovement(F32 dt) const;
};
}

#endif
