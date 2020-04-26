#ifndef __SCRIPTING_COMPONENT_H__
#define __SCRIPTING_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct ScriptingComponent : public Component {

    ScriptingComponent();

    py::module m_botModule;
};
}

#endif
