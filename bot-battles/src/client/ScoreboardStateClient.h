#ifndef __SCOREBOARD_STATE_CLIENT_H__
#define __SCOREBOARD_STATE_CLIENT_H__

#include "State.h"

namespace sand {

struct ScoreboardComponent;

//----------------------------------------------------------------------------------------------------
class ScoreboardStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::SCOREBOARD;
    }

public:
    const char* GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

private:
    void RenderResultsGui(ScoreboardComponent& scoreboardComponent) const;
    void RenderRestartGui(ScoreboardComponent& scoreboardComponent) const;
};
}

#endif
