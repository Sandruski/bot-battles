#ifndef __HUD_SYSTEM_H__
#define __HUD_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class HUDSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::HUD;
    }

public:
    HUDSystem();

    bool StartUp() override;
    bool PreUpdate() override;
    bool Render() override;

private:
    Entity m_fps;
};
}

#endif
