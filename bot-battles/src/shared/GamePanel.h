#ifndef __GAME_PANEL_H__
#define __GAME_PANEL_H__

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GamePanel : public Panel {
public:
    static DebugOptionsPanelType GetType()
    {
        return DebugOptionsPanelType::GAME;
    }

public:
    std::string GetName() const override;

    bool RenderHeader() const override;
    bool RenderBody() const override;
};
}

#endif
