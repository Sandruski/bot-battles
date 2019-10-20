#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

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
	SERVER,
	CLIENT,

	COUNT,
	INVALID
};

using Signature = std::bitset<static_cast<std::size_t>(ComponentType::COUNT)>;
}

#endif
