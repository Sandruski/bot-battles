#include "MoveList.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MoveList::MoveList()
    : m_moves()
    , m_lastMoveTimestamp(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
MoveList::~MoveList()
{
}

//----------------------------------------------------------------------------------------------------
const Move& MoveList::AddMove(const InputState& inputState, F32 timestamp)
{
    float dt = m_lastMoveTimestamp > 0.0f ? timestamp - m_lastMoveTimestamp : 0.0f;

    m_moves.emplace_back(inputState, timestamp, dt);
    m_lastMoveTimestamp = timestamp;

    return m_moves.back();
}

//----------------------------------------------------------------------------------------------------
void MoveList::Clear()
{
    m_moves.clear();
}
}
