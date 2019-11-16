#ifndef __SINGLETON_INPUT_COMPONENT_H__
#define __SINGLETON_INPUT_COMPONENT_H__

#include "Move.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SingletonInputComponent {

    SingletonInputComponent()
        : m_moves()
        , m_inputTime(0.03f)
        , m_lastTime(0.0f)
    {
    }
    ~SingletonInputComponent()
    {
    }

    const Move& AddMove(const InputComponent& input, F32 timestamp);

    const Move& GetMove(U32 index) const
    {
        return m_moves.at(index);
    }

    U32 GetMoveCount() const
    {
        return static_cast<U32>(m_moves.size());
    }

    bool HasMoves() const
    {
        return !m_moves.empty();
    }

    void ClearMoves()
    {
        m_moves.clear();
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
