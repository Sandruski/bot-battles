#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

#include <cstddef>

namespace sand
{

	using ComponentID = std::size_t;

	enum class ComponentType
	{
		TRANSFORM,
		SPRITE,

		RENDERER,
		INPUT,

		COMPONENT_COUNT,
		COMPONENT_INVALID
	};

	const std::size_t MAX_COMPONENTS = static_cast<ComponentID>(ComponentType::COMPONENT_COUNT);
	const ComponentID INVALID_COMPONENT = MAX_COMPONENTS;
}

#endif