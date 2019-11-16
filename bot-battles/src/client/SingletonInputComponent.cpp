#include "SingletonInputComponent.h"

#include "Move.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
const Move& SingletonInputComponent::AddMove(const InputComponent& input, F32 timestamp)
{
    float lastMoveTimestamp = !m_moves.empty() ? m_moves.back().GetTimestamp() : 0.0f;
    float dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;

    return m_moves.emplace_back(input, timestamp, dt);
}
}
