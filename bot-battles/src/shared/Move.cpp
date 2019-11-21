#include "Move.h"

#include "MemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Move::Move()
    : m_input()
    , m_timestamp(0.0f)
    , m_dt(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Move::Move(const InputComponent& input, F32 timestamp, F32 dt)
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
    m_input.Write(outputStream, memberFlags); // TODO CHANGE!
    outputStream.Write(m_timestamp);
}

//----------------------------------------------------------------------------------------------------
void Move::Read(InputMemoryStream& inputStream)
{
    m_input.Read(inputStream, 0); // TODO CHANGE!
    inputStream.Read(m_timestamp);
}
}
