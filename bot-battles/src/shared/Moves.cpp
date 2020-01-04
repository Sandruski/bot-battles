#include "Moves.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Moves::Moves()
    : m_moves()
{
}

//----------------------------------------------------------------------------------------------------
const Move& Moves::AddMove(const InputComponent& input, U32 dirtyState, F32 timestamp)
{
    F32 lastMoveTimestamp = GetLastMoveTimestamp();
    F32 dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;
    return m_moves.emplace_back(input, dirtyState, timestamp, dt);
}

//----------------------------------------------------------------------------------------------------
const Move& Moves::AddMove(const Move& move)
{
    F32 lastMoveTimestamp = GetLastMoveTimestamp();
    F32 timestamp = move.GetTimestamp();
    F32 dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;
    return m_moves.emplace_back(move.GetInput(), move.GetDirtyState(), timestamp, dt);
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
F32 Moves::GetLastMoveTimestamp() const
{
    return HasMoves() ? m_moves.back().GetTimestamp() : 0.0f;
}

//----------------------------------------------------------------------------------------------------
void Moves::RemoveMoves(F32 lastMoveTimestamp)
{
    while (!m_moves.empty() && m_moves.front().GetTimestamp() <= lastMoveTimestamp) {
        m_moves.pop_front();
    }
}

//----------------------------------------------------------------------------------------------------
void Moves::ClearMoves()
{
    m_moves.clear();
}
}
