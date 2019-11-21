#include "SingletonInputComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
const Move& SingletonInputComponent::AddMove(const InputComponent& input, U32 dirtyState, F32 timestamp)
{
    float lastMoveTimestamp = !m_moves.empty() ? m_moves.back().GetTimestamp() : 0.0f;
    float dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;

    return m_moves.emplace_back(input, dirtyState, timestamp, dt);
}
}
