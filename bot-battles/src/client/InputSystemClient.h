#ifndef __INPUT_SYSTEM_CLIENT_H__
#define __INPUT_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class InputSystemClient : public System {
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
