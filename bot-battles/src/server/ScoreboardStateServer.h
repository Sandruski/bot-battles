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
    const char* GetName() const override;

    bool Enter() const override;
    bool Exit() const override;
};
}

#endif
