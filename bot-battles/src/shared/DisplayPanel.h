#ifndef __DISPLAY_PANEL_H__
#define __DISPLAY_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class DisplayPanel : public Panel {
public:
    static SettingsPanelType GetType()
    {
        return SettingsPanelType::DISPLAY;
    }

public:
    std::string GetName() const override;

    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
