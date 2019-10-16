#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__

#include "ComponentDefs.h"
#include "Memory.h"
#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class InputSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::INPUT;
    }

public:
    InputSystem();
    ~InputSystem() override;

    bool StartUp() override;
    bool Update(F32 dt) override;
    bool ShutDown() override;
};
}

#endif
