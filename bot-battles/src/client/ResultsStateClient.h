#ifndef __RESULTS_STATE_CLIENT_H__
#define __RESULTS_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ResultsStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::RESULTS;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToRestart() const; // if button
    void ChangeToMainMenu() const; // if timer or button or disconnected or server disconnected
};
}

#endif
