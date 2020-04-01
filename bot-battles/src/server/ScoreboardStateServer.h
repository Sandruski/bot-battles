#ifndef __SCOREBOARD_STATE_SERVER_H__
#define __SCOREBOARD_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ScoreboardStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::SCOREBOARD;
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
