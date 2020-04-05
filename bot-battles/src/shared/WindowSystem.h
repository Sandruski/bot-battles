#ifndef __WINDOW_SYSTEM_H__
#define __WINDOW_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class WindowSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::WINDOW;
    }

public:
    bool StartUp() override;
    bool ShutDown() override;
};
}

#endif
