#ifndef __MOVE_COMPONENT_H__
#define __MOVE_COMPONENT_H__

#include "Component.h"
#include "Moves.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MoveComponent : public Component {

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
