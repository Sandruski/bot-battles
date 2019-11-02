#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

#ifdef _CLIENT
enum class SystemType {
    CLIENT,

    WINDOW,
    RENDERER,
    EVENT,

    COUNT,
    INVALID
};
#elif _SERVER
enum class SystemType {
    SERVER,

    MOVEMENT,

    COUNT,
    INVALID
};
#endif
}

#endif
