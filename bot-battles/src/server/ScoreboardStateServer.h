#ifndef __SCOREBOARD_STATE_SERVER_H__
#define __SCOREBOARD_STATE_SERVER_H__

#include "State.h"

namespace sand {

struct ScoreboardComponent;

//----------------------------------------------------------------------------------------------------
class ScoreboardStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::SCOREBOARD;
    }

public:
    const char* GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool Exit() const override;

private:
    void UpdateResults(ScoreboardComponent& scoreboardComponent) const;
    void UpdateRestart() const;
};
}

#endif
