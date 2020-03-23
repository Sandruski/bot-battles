#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GuiSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::GUI;
    }

public:
    GuiSystem();

    bool StartUp() override;
    bool Update() override;
    bool PreRender() override;
    bool Render() override;
    bool PostRender() override;
    bool ShutDown() override;
};
}

#endif
