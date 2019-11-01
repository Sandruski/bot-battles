#ifndef __MOVE_LIST_H__
#define __MOVE_LIST_H__

#include "Move.h"

namespace sand {

class InputState;

//----------------------------------------------------------------------------------------------------
class MoveList {
public:
    MoveList();
    ~MoveList();

    const Move& AddMove(const InputState& inputState, F32 timestamp);
    void Clear();

    const std::deque<Move>& GetMoves() const
    {
        return m_moves;
    }

private:
    std::deque<Move> m_moves;

    F32 m_lastMoveTimestamp;
};
}

#endif
