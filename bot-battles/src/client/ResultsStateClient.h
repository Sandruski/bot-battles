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
    void ChangeToRestart() const;
    void ChangeToMainMenu() const;
};
}

#endif
