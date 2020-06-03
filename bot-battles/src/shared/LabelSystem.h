#ifndef __LABEL_SYSTEM_H__
#define __LABEL_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class LabelSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::LABEL;
    }

public:
    LabelSystem();

    bool RenderGui() override;
};
}

#endif
