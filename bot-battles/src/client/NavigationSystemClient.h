#ifndef __NAVIGATION_SYSTEM_CLIENT_H__
#define __NAVIGATION_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class NavigationSystemClient : public System {
public:
    static SystemType GetType()
    {
        return SystemType::NAVIGATION;
    }

public:
    NavigationSystemClient();

    bool Update() override;
};
}

#endif
