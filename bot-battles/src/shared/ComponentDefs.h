#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

#include "ComponentTypes.h"

namespace sand {

const U32 MAX_COMPONENTS = static_cast<U32>(ComponentType::COUNT);
const U32 INVALID_COMPONENT = static_cast<U32>(ComponentType::INVALID);

using Signature = std::bitset<MAX_COMPONENTS>;
}

#endif
