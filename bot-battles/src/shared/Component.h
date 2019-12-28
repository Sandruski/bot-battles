#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "NetworkableObject.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct Component : public NetworkableObject {

    Component();
    virtual ~Component() = default;

    bool m_isEnabled;
};
}

#endif
