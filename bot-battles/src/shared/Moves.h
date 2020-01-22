#ifndef __MOVES_H__
#define __MOVES_H__

#include "Move.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Moves {
public:
    Moves();

    const Move& AddMove(const InputComponent& input, U32 dirtyState);
    const Move& AddMove(const Move& move);
    void RemoveMoves(U32 lastMoveFrame);
    void ClearMoves();

    const Move& GetMove(U32 index) const;
    const Move& GetLastMove() const;
    bool HasMoves() const;
    U32 GetMoveCount() const;

private:
    std::deque<Move> m_moves;
};
}

#endif
