#ifndef __BOT_SYSTEM_H__
#define __BOT_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class BotSystem : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::BOT;
    }

public:
    BotSystem();

    bool PreUpdate() override;
    bool Update() override;
};
}

#endif
