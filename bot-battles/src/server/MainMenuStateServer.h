#ifndef __MAIN_MENU_STATE_SERVER_H__
#define __MAIN_MENU_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MainMenuStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::MAIN_MENU;
    }

public:
    MainMenuStateServer();

    const char* GetName() override;

    bool Enter() override;
    bool RenderGui() override;
    bool Exit() override;
};
}

#endif
