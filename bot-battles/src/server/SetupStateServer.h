#ifndef __SETUP_STATE_SERVER_H__
#define __SETUP_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SetupStateServer : public State {
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

private:
    void ChangeToConnect() const; // if button
};
}

#endif
