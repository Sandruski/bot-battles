#ifndef __NETWORKING_PANEL_H__
#define __NETWORKING_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class NetworkingPanel : public Panel {
public:
    static DebugOptionsPanelType GetType()
    {
        return DebugOptionsPanelType::NETWORKING;
    }

public:
    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
