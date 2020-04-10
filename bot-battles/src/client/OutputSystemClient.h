#ifndef __OUTPUT_SYSTEM_CLIENT_H__
#define __OUTPUT_SYSTEM_CLIENT_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class OutputSystemClient : public System {
public:
    static SystemType GetType()
    {
        return SystemType::OUTPUT;
    }

public:
    OutputSystemClient();

    bool Update() override;
};
}

#endif
