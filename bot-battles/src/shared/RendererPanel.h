#ifndef __RENDERER_PANEL_H__
#define __RENDERER_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RendererPanel : public Panel {
public:
    static DebugOptionsPanelType GetType()
    {
        return DebugOptionsPanelType::RENDERER;
    }

public:
    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
