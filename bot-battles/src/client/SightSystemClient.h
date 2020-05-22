#ifndef __SIGHT_SYSTEM_CLIENT_H__
#define __SIGHT_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SightSystemClient : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::SIGHT;
    }

public:
    SightSystemClient();

    bool DebugRender() override;
};
}

#endif
