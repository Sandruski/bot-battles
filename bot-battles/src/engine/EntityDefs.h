#ifndef __ENTITY_DEFS_H__
#define __ENTITY_DEFS_H__

#include <cstddef>

namespace sand
{

	using EntityID = std::size_t;

	const std::size_t MAX_ENTITIES = 1024;
	const EntityID INVALID_ENTITY = MAX_ENTITIES;
}

#endif