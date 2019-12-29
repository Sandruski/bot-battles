#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__

#include "System.h"

namespace sand {

struct MoveComponent;

//----------------------------------------------------------------------------------------------------
class InputSystem : public System {
public:
    enum class KeyState {
        IDLE,
        DOWN,
        REPEAT,
        UP
    };
    KeyState m_keyboard[SDL_NUM_SCANCODES];

    static SystemType GetType()
    {
        return SystemType::INPUT;
    }

public:
    InputSystem();

    bool Update() override;

    void UpdateSampleInput(MoveComponent& moveComponent) const;
    void SampleInput(MoveComponent& moveComponent, F32 timestamp) const;
};
}

#endif
