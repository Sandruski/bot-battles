#include "Move.h"

#include "MemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Move::Move()
    : m_input()
    , m_dirtyState(0)
    , m_timestamp(0.0f)
    , m_dt(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Move::Move(const InputComponent& input, U32 dirtyState, F32 timestamp, F32 dt)
    : m_input(input)
    , m_dirtyState(dirtyState)
    , m_timestamp(timestamp)
    , m_dt(dt)
{
}

//----------------------------------------------------------------------------------------------------
Move::~Move()
{
}

//----------------------------------------------------------------------------------------------------
void Move::Write(OutputMemoryStream& outputStream) const
{
    outputStream.Write(m_dirtyState); // TODO CHANGE! This dirty state is alone :c
    m_input.Write(outputStream, m_dirtyState);
    outputStream.Write(m_timestamp);
}

//----------------------------------------------------------------------------------------------------
void Move::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_dirtyState); // TODO CHANGE! This dirty state is alone :c
    m_input.Read(inputStream, m_dirtyState);
    inputStream.Read(m_timestamp);
}
}
