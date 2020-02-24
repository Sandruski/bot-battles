#ifndef __GAMEPLAY_DEFS_H__
#define __GAMEPLAY_DEFS_H__

namespace sand {

    enum class PhaseType : U16 {
        START,
        PLAY,
        END
    };

    const U32 PLAYERS_PER_MATCH = 2;
}

#endif