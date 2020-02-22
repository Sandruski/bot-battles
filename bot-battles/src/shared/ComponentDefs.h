#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

#include "ComponentTypes.h"

namespace sand {

const U16 MAX_COMPONENTS = static_cast<U16>(ComponentType::COUNT);
#ifdef _CLIENT
const U16 MAX_NETWORK_COMPONENTS = MAX_COMPONENTS - 2;
#elif defined(_SERVER)
const U16 MAX_NETWORK_COMPONENTS = MAX_COMPONENTS;
#endif
const U16 INVALID_COMPONENT = MAX_COMPONENTS;

using Signature = U16;
}

#endif
