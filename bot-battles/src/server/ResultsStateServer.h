#ifndef __RESULTS_STATE_SERVER_H__
#define __RESULTS_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ResultsStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::RESULTS;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool RenderGui() const override;
    bool Exit() const override;

private:
    void ChangeToRestart() const; // if button
    void ChangeToMainMenu() const; // if button (TODO: why do clients have timers?)
};
}

#endif
