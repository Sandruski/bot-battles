#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

#include "ComponentTypes.h"

namespace sand {

const U16 MAX_COMPONENTS = static_cast<U16>(ComponentType::COUNT);
const U16 INVALID_COMPONENT = static_cast<U16>(ComponentType::INVALID);

using Signature = U16;
}

#endif
