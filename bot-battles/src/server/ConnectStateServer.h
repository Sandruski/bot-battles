#ifndef __CONNECT_STATE_SERVER_H__
#define __CONNECT_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ConnectStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::CONNECT;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void ChangeToGameplay() const; // auto
};
}

#endif
