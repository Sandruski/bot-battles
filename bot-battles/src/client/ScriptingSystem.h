#ifndef __SCRIPTING_SYSTEM_CLIENT_H__
#define __SCRIPTING_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ScriptingSystemClient : public System {
public:
    static SystemType GetType()
    {
        return SystemType::SCRIPTING;
    }

public:
    bool StartUp() override;
    bool Update() override;
};
}

#endif
