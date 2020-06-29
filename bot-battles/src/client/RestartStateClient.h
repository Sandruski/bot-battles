#ifndef __RESTART_STATE_CLIENT_H__
#define __RESTART_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RestartStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::RESTART;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToGameplay() const;
    void ChangeToResults() const;
    void ChangeToMainMenu() const;
};
}

#endif
