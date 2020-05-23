#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__

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
    bool Update() override;
};
}

#endif
