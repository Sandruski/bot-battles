#ifndef __START_STATE_SERVER_H__
#define __START_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class StartStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::START;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void OnPlayerAdded() const;

    void ChangeToPlay() const;
    void ChangeToMainMenu() const;
};
}

#endif
