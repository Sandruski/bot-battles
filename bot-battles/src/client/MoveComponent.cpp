#include "MoveComponent.h"

#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const Move& MoveComponent::AddMove(const InputComponent& input, U32 dirtyState, F32 timestamp)
{
    float lastMoveTimestamp = GetLastMoveTimestamp();
    float dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;

    return m_moves.emplace_back(input, dirtyState, timestamp, dt);
}

//----------------------------------------------------------------------------------------------------
const Move& MoveComponent::GetMove(U32 index) const
{
    return m_moves.at(index);
}

//----------------------------------------------------------------------------------------------------
F32 MoveComponent::GetLastMoveTimestamp() const
{
    return !m_moves.empty() ? m_moves.back().GetTimestamp() : 0.0f;
}
}
