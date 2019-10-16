#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

#include "Memory.h"

#include <bitset>

namespace sand {

enum class ComponentType {
    TRANSFORM,
    SPRITE,

    COUNT,
    INVALID
};

enum class SingletonComponentType {
	WINDOW,
	RENDERER,
	INPUT,

	COUNT,
	INVALID
};

using Signature = std::bitset<static_cast<std::size_t>(ComponentType::COUNT)>;
}

#endif
