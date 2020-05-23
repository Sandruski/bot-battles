#ifndef __SIGHT_SYSTEM_CLIENT_H__
#define __SIGHT_SYSTEM_CLIENT_H__

#include "SightSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SightSystemClient : public SightSystem, public Subject {
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
