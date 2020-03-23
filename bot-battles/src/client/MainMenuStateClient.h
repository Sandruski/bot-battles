#ifndef __MAIN_MENU_STATE_CLIENT_H__
#define __MAIN_MENU_STATE_CLIENT_H__

#include "State.h"

namespace sand {

struct MainMenuComponent;

//----------------------------------------------------------------------------------------------------
class MainMenuStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::MAIN_MENU;
    }

public:
    const char* GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

private:
    void RenderStartGui(MainMenuComponent& mainMenuComponent) const;
    void RenderConnectGui(MainMenuComponent& mainMenuComponent) const;
};
}

#endif
