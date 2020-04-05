#ifndef __STATES_PANEL_H__
#define __STATES_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class StatesPanel : public Panel {
public:
    static PanelType GetType()
    {
        return PanelType::STATES;
    }

public:
    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
