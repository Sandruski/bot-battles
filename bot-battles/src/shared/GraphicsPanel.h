#ifndef __GRAPHICS_PANEL_H__
#define __GRAPHICS_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GraphicsPanel : public Panel {
public:
    static DebugOptionsPanelType GetType()
    {
        return DebugOptionsPanelType::GRAPHICS;
    }

public:
    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
