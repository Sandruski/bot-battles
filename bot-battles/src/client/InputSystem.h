#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__

#include "System.h"

namespace sand {

struct SingletonInputComponent;

//----------------------------------------------------------------------------------------------------
class InputSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::INPUT;
    }

public:
    InputSystem();
    ~InputSystem() override;

    bool Update() override;

    void UpdateSampleInput(SingletonInputComponent& input) const;
    void SampleInput(SingletonInputComponent& input) const;
};
}

#endif
