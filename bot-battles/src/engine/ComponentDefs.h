#ifndef __COMPONENT_DEFS_H__
#define __COMPONENT_DEFS_H__

#include <cstddef>

namespace sand
{

	using ComponentID = std::size_t;

	enum ComponentType
	{
		TRANSFORM,
		SPRITE,

		COMPONENT_COUNT,
		COMPONENT_INVALID
	};

	const ComponentID MAX_COMPONENTS = ComponentType::COMPONENT_COUNT;
	const ComponentID INVALID_COMPONENT = MAX_COMPONENTS;
}

#endif