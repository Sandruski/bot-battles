#ifndef __MOVE_COMPONENT_H__
#define __MOVE_COMPONENT_H__

#include "Moves.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MoveComponent {

    MoveComponent();

    F32 GetNextMoveTime() const
    {
        return m_lastMoveTimestamp + TIME_BETWEEN_MOVE;
    }

    Moves m_moves;

    F32 m_lastMoveTimestamp;
};
}

#endif
