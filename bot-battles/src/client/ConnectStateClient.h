#ifndef __CONNECT_STATE_CLIENT_H__
#define __CONNECT_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ConnectStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::CONNECT;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool RenderGui() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToGameplay() const; // WELCOME
    void ChangeToSetup() const; // if button or disconnected or server disconnected
};
}

#endif
