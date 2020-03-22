#ifndef __GAMEPLAY_STATE_SERVER_H__
#define __GAMEPLAY_STATE_SERVER_H__

#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GameplayStateServer : public State, public Observer {
public:
    static StateType GetType()
    {
        return StateType::GAMEPLAY;
    }

public:
    GameplayStateServer();

    const char* GetName() override;

    bool Enter() override;
    bool Exit() override;

    void OnNotify(const Event& event) override;

private:
    void OnPlayerAdded() const;
    void OnPlayerRemoved() const;
};
}

#endif
