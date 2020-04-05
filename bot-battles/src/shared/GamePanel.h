#ifndef __GAME_PANEL_H__
#define __GAME_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GamePanel : public Panel {
public:
    static PanelType GetType()
    {
        return PanelType::GAME;
    }

public:
    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
