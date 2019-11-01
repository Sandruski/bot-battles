#include "InputState.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputState::InputState()
    : m_desiredRightAmount(0.0f)
    , m_desiredLeftAmount(0.0f)
    , m_desiredForwardAmount(0.0f)
    , m_desiredBackAmount(0.0f)
    , m_isShooting(false)
{
}

//----------------------------------------------------------------------------------------------------
InputState::~InputState()
{
}

//----------------------------------------------------------------------------------------------------
F32 InputState::GetDesiredHorizontalDelta() const
{
    return m_desiredRightAmount - m_desiredLeftAmount;
}

//----------------------------------------------------------------------------------------------------
F32 InputState::GetDesiredVerticalDelta() const
{
    return m_desiredForwardAmount - m_desiredBackAmount;
}

//----------------------------------------------------------------------------------------------------
bool InputState::Write(OutputMemoryStream& /*outputStream*/) const
{
    return false;
}

//----------------------------------------------------------------------------------------------------
bool InputState::Read(InputMemoryStream& /*inputStream*/)
{
    return false;
}
}
