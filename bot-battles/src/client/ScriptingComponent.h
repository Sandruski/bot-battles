#ifndef __SCRIPTING_COMPONENT_H__
#define __SCRIPTING_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct ScriptingComponent : public Component {

    ScriptingComponent();

    py::scoped_interpreter m_scopedInterpreter;
    py::module m_mainModule;
    ;
    py::module m_myBotModule;
};
}

#endif
