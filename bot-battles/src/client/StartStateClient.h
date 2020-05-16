#ifndef __START_STATE_CLIENT_H__
#define __START_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class StartStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::START;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToPlay() const; // PLAY
    void ChangeToMainMenu() const; // if disconnected or server disconnected
};
}

#endif
