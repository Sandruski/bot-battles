#ifndef __GAMEPLAY_STATE_H__
#define __GAMEPLAY_STATE_H__

#include "State.h"

namespace sand {

struct TransformComponent;

//----------------------------------------------------------------------------------------------------
class GameplayState : public State {
public:
    GameplayState();
    ~GameplayState() override;

    bool Create() override;
    bool Destroy() override;

    bool Enter() override;
    bool PreUpdate() override;
    bool Update() override;
    bool PostUpdate() override;
    bool Render() override;
    bool Exit() override;

private:
    std::shared_ptr<TransformComponent> m_transform;
};
}

#endif
