#ifndef __EVENT_SYSTEM_H__
#define __EVENT_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class EventSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::EVENT;
    }

public:
    EventSystem();
    ~EventSystem() override;

    bool StartUp() override;
    bool Update() override;
    bool ShutDown() override;
};
}

#endif
