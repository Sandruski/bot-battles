#ifndef __MAIN_MENU_STATE_CLIENT_H__
#define __MAIN_MENU_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MainMenuStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::MAIN_MENU;
    }

public:
    MainMenuStateClient();

    const char* GetName() override;

    bool Enter() override;
    bool Update() override;
    bool RenderGui() override;
    bool Exit() override;
};
}

#endif
