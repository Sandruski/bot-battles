#ifndef __MAIN_MENU_STATE_H__
#define __MAIN_MENU_STATE_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MainMenuState : public State {
public:
    static StateType GetType()
    {
        return StateType::MAIN_MENU;
    }

public:
    MainMenuState();

    const char* GetName() override;

    bool Enter() override;
    bool PreUpdate() override;
    bool Update() override;
    bool PostUpdate() override;
    bool Exit() override;
};
}

#endif
