#ifndef __INPUT_SYSTEM_CLIENT_H__
#define __INPUT_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

struct SingletonInputComponent;

//----------------------------------------------------------------------------------------------------
class InputSystemClient : public System {
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
    InputSystemClient();
    ~InputSystemClient() override;

    bool Update() override;

    void UpdateSampleInput(SingletonInputComponent& singletonInput) const;
    void SampleInput(SingletonInputComponent& singletonInput, F32 timestamp) const;
};
}

#endif
