#ifndef __SYSTEM_DEFS_H__
#define __SYSTEM_DEFS_H__

namespace sand {

enum class SystemType {
	WINDOW,
    RENDERER,
    INPUT,
	SERVER,
	CLIENT,

    COUNT,
    INVALID
};
}

#endif
