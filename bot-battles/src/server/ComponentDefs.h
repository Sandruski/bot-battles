#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

namespace sand {

enum class ComponentType {
    TRANSFORM,

    COUNT,
    INVALID
};

enum class SingletonComponentType {
	INPUT,
	SERVER,

	COUNT,
	INVALID
};

using Signature = std::bitset<static_cast<std::size_t>(ComponentType::COUNT)>;
}

#endif
