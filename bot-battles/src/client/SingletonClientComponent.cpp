#include "SingletonClientComponent.h"

#include "Move.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
const Move& SingletonClientComponent::AddMove(const SingletonInputComponent& input, F32 timestamp)
{
    float lastMoveTimestamp = !m_moves.empty() ? m_moves.back().GetTimestamp() : 0.0f;
    float dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;

    return m_moves.emplace_back(input, timestamp, dt);
}

//----------------------------------------------------------------------------------------------------
const Move& SingletonClientComponent::GetMove(U32 index) const
{
    return m_moves.at(index);
}
}
