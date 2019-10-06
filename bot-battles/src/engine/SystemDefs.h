#ifndef __SYSTEM_DEFS_H__
#define __SYSTEM_DEFS_H__

#include <cstddef>

namespace sand
{

	enum class SystemType
	{
		RENDERER,
		INPUT,

		SYSTEM_COUNT,
		SYSTEM_INVALID
	};

	const std::size_t MAX_SYSTEMS = static_cast<std::size_t>(SystemType::SYSTEM_COUNT);
}

#endif