#include "MoveComponent.h"

#include "InputComponent.h"

namespace sand {
MoveComponent::MoveComponent()
    : m_moves()
    , m_lastMoveTimestamp(0.0f)
{
}
}
