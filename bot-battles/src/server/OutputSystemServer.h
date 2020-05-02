#ifndef __OUTPUT_SYSTEM_SERVER_H__
#define __OUTPUT_SYSTEM_SERVER_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class OutputSystemServer : public System {
public:
    static SystemType GetType()
    {
        return SystemType::OUTPUT;
    }

public:
    bool PostUpdate() override;
};
}

#endif
