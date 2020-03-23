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
    const char* GetName() const override;

    bool Enter() const override;
    bool Exit() const override;

    void OnNotify(const Event& event) override;

private:
    void OnPlayerAdded() const;
    void OnPlayerRemoved() const;
};
}

#endif
