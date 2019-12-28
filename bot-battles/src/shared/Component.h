#ifndef __COMPONENT_H__
#define __COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct Component {

    Component();
    virtual ~Component() = default;

    bool m_isEnabled;
};
}

#endif
