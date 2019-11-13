#include "Move.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Move::Move(const SingletonInputComponent& input, F32 timestamp, F32 dt)
    : m_input(input)
    , m_timestamp(timestamp)
    , m_dt(dt)
{
}

//----------------------------------------------------------------------------------------------------
Move::~Move()
{
}

//----------------------------------------------------------------------------------------------------
void Move::Write(OutputMemoryStream& outputStream, U16 memberFlags) const
{
    m_input.Write(outputStream, memberFlags);
    outputStream.Write(m_timestamp);
}

//----------------------------------------------------------------------------------------------------
void Move::Read(InputMemoryStream& inputStream)
{
    m_input.Read(inputStream);
    inputStream.Read(m_timestamp);
}
}
