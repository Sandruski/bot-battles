#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GUISystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::GUI;
    }

public:
    bool StartUp() override;
    bool Update() override;
    bool PreRender() override;
    bool RenderGui() override;
    bool ShutDown() override;
};
}

#endif
