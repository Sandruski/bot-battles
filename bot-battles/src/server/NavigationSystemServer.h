#ifndef __NAVIGATION_SYSTEM_SERVER_H__
#define __NAVIGATION_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class NavigationSystemServer : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::NAVIGATION;
    }

public:
    NavigationSystemServer();

    bool Update() override;
};
}

#endif
