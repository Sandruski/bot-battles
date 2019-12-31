#ifndef __MOVE_COMPONENT_H__
#define __MOVE_COMPONENT_H__

#include "Move.h"

namespace sand {

struct InputComponent;

//----------------------------------------------------------------------------------------------------
// System Component
struct MoveComponent {

    MoveComponent()
        : m_moves()
        , m_inputTime(0.03f)
        , m_lastTime(0.0f)
    {
    }

    const Move& AddMove(const InputComponent& input, U32 dirtyState, F32 timestamp);
    const Move& GetMove(U32 index) const;
    void RemoveProcessedMoves(F32 lastMoveTimestamp);
    F32 GetLastMoveTimestamp() const;

    U32 GetMoveCount() const
    {
        return static_cast<U32>(m_moves.size());
    }

    bool HasMoves() const
    {
        return !m_moves.empty();
    }

    F32 GetNextInputTime() const
    {
        return m_lastTime + m_inputTime;
    }

    std::deque<Move> m_moves;

    F32 m_inputTime;
    F32 m_lastTime;
};
}

#endif
