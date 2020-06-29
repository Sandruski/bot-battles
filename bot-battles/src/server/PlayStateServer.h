#ifndef __PLAY_STATE_SERVER_H__
#define __PLAY_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class PlayStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::PLAY;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void OnHealthHurt() const;

    void ChangeToScoreboard() const;
};
}

#endif
