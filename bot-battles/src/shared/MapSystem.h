#ifndef __MAP_SYSTEM_H__
#define __MAP_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MapSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::MAP;
    }

public:
    bool DebugRender() override;
};
}

#endif
