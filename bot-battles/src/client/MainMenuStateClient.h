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
    std::string GetName() const override;

    bool Create() const override;
    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;
};
}

#endif
