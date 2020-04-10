#ifndef __STATES_PANEL_H__
#define __STATES_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class StatesPanel : public Panel {
public:
    static DebugOptionsPanelType GetType()
    {
        return DebugOptionsPanelType::STATES;
    }

public:
    std::string GetName() const override;

    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
