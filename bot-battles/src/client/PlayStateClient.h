#ifndef __PLAY_STATE_CLIENT_H__
#define __PLAY_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class PlayStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::PLAY;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToScoreboard() const; // RESULTS (game end or another player disconnected and game was forced to end abruptly)
    void ChangeToMainMenu() const; // if disconnected or server disconnected
};
}

#endif
