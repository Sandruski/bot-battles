#ifndef __SETUP_STATE_CLIENT_H__
#define __SETUP_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SetupStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::SETUP;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToConnect() const;
};
}

#endif
