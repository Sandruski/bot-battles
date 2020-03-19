#ifndef __MAP_RENDERER_SYSTEM_H__
#define __MAP_RENDERER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MapRendererSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::MAP_RENDERER;
    }

public:
    MapRendererSystem();

    bool Render() override;
};
}

#endif
