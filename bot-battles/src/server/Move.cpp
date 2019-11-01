#include "Move.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Move::Move(const InputState& inputState, F32 timestamp, F32 dt)
    : m_inputState(inputState)
    , m_timestamp(timestamp)
    , m_dt(dt)
{
}

//----------------------------------------------------------------------------------------------------
Move::~Move()
{
}

//----------------------------------------------------------------------------------------------------
bool Move::Write(OutputMemoryStream& /*outputStream*/) const
{
    return false;
}

//----------------------------------------------------------------------------------------------------
bool Move::Read(InputMemoryStream& /*inputStream*/)
{
    return false;
}
}
