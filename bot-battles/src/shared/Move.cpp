#include "Move.h"

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

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void Move::Write(OutputMemoryStream& outputStream) const
{
    outputStream.Write(m_dirtyState); // TODO CHANGE! This dirty state is alone :c
    m_input.Write(outputStream, m_dirtyState);
    outputStream.Write(m_timestamp);
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
void Move::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_dirtyState); // TODO CHANGE! This dirty state is alone :c
    m_input.Read(inputStream, m_dirtyState, ReplicationActionType::NONE, INVALID_ENTITY);
    inputStream.Read(m_timestamp);
}
#endif

//----------------------------------------------------------------------------------------------------
const InputComponent& Move::GetInput() const
{
    return m_input;
}

//----------------------------------------------------------------------------------------------------
U32 Move::GetDirtyState() const
{
    return m_dirtyState;
}

//----------------------------------------------------------------------------------------------------
F32 Move::GetTimestamp() const
{
    return m_timestamp;
}

//----------------------------------------------------------------------------------------------------
F32 Move::GetDt() const
{
    return m_dt;
}
}
