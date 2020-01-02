#ifndef __MOVES_H__
#define __MOVES_H__

#include "Move.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Moves {
public:
    Moves();

    const Move& AddMove(const InputComponent& input, U32 dirtyState, F32 timestamp);
    const Move& AddMove(const Move& move);
    void RemoveMoves(F32 lastMoveTimestamp);
    void ClearMoves();

    const Move& GetMove(U32 index) const;
    const Move& GetLastMove() const;
    bool HasMoves() const;
    U32 GetMoveCount() const;

    F32 GetLastMoveTimestamp() const;

private:
    std::deque<Move> m_moves;
    F32 m_lastMoveTimestamp;
};
}

#endif
