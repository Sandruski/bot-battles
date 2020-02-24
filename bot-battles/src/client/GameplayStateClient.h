#ifndef __GAMEPLAY_STATE_CLIENT_H__
#define __GAMEPLAY_STATE_CLIENT_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GameplayStateClient : public State {
public:
    static StateType GetType()
    {
        return StateType::GAMEPLAY;
    }

public:
    GameplayStateClient();

    const char* GetName() override;

    bool Enter() override;
    bool PreUpdate() override;
    bool Update() override;
    bool PostUpdate() override;
    bool Exit() override;
};
}

#endif
