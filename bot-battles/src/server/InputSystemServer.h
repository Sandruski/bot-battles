#ifndef __INPUT_SYSTEM_SERVER_H__
#define __INPUT_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class InputSystemServer : public System {
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
