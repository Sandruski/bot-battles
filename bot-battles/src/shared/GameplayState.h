#ifndef __GAMEPLAY_STATE_H__
#define __GAMEPLAY_STATE_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GameplayState : public State, public Observer {
public:
    static StateType GetType()
    {
        return StateType::GAMEPLAY;
    }

public:
    GameplayState();

    void OnNotify(const Event& event) override;

    const char* GetName() override;

    bool Enter() override;
    bool PreUpdate() override;
    bool Update() override;
    bool PostUpdate() override;
    bool Exit() override;

private:
    void OnPlayerAdded();
    void OnPlayerRemoved();
};
}

#endif
