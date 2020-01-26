#include "Moves.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Moves::Moves()
    : m_moves()
{
}

//----------------------------------------------------------------------------------------------------
const Move& Moves::AddMove(const InputComponent& input, U32 dirtyState)
{
    // TODO: frame or internal counter which can also be used to retrieve the last move frame?
    return m_moves.emplace_back(input, dirtyState, 1.0f, Time::GetInstance().GetFrame());
}

//----------------------------------------------------------------------------------------------------
const Move& Moves::AddMove(const Move& move)
{
    return m_moves.emplace_back(move);
}

//----------------------------------------------------------------------------------------------------
const Move& Moves::GetMove(U32 index) const
{
    return m_moves.at(index);
}

//----------------------------------------------------------------------------------------------------
const Move& Moves::GetLastMove() const
{
    return m_moves.back();
}

//----------------------------------------------------------------------------------------------------
bool Moves::HasMoves() const
{
    return !m_moves.empty();
}

//----------------------------------------------------------------------------------------------------
U32 Moves::GetMoveCount() const
{
    return m_moves.size();
}

//----------------------------------------------------------------------------------------------------
void Moves::RemoveMoves(U32 lastMoveFrame)
{
    while (!m_moves.empty() && m_moves.front().GetFrame() <= lastMoveFrame) {
        m_moves.pop_front();
    }
}

//----------------------------------------------------------------------------------------------------
void Moves::ClearMoves()
{
    m_moves.clear();
}
}
