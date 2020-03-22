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
    const char* GetName() const override;

    bool Enter() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void RenderStartGui() const;
    void RenderConnectGui() const;

    void OnPlayerAdded() const;
};
}

#endif
