#ifndef __GAME_DEFS_H__
#define __GAME_DEFS_H__

namespace sand {

enum class PhaseType : U16 {

    START,
    CONNECT,
    WAIT,
    PLAY,
    END,
    RESTART
};
}

#endif
