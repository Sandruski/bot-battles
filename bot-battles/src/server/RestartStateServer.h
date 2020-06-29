#ifndef __RESTART_STATE_SERVER_H__
#define __RESTART_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RestartStateServer : public State {
public:
    static StateType GetType()
    {
        return StateType::RESTART;
    }

public:
    std::string GetName() const override;

    bool Enter() const override;
    bool Update() const override;
    bool Exit() const override;

private:
    void ChangeToGameplay() const;
};
}

#endif
