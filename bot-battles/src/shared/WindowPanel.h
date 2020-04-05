#ifndef __WINDOW_PANEL_H__
#define __WINDOW_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WindowPanel : public Panel {
public:
    static PanelType GetType()
    {
        return PanelType::WINDOW;
    }

public:
    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
