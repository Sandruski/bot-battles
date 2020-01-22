#include "Move.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Move::Move()
    : m_inputComponent()
    , m_dirtyState(0)
    , m_dt(0.0f)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Move::Move(const InputComponent& input, U32 dirtyState, F32 dt, U32 frame)
    : m_inputComponent(input)
    , m_dirtyState(dirtyState)
    , m_dt(dt)
    , m_frame(frame)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void Move::Write(OutputMemoryStream& outputStream) const
{
    outputStream.Write(m_dirtyState);
    m_inputComponent.Write(outputStream, m_dirtyState);
    outputStream.Write(m_dt);
    outputStream.Write(m_frame);
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
void Move::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_dirtyState);
    m_inputComponent.Read(inputStream, m_dirtyState, ReplicationActionType::NONE, INVALID_ENTITY);
    inputStream.Read(m_dt);
    inputStream.Read(m_frame);
}
#endif

//----------------------------------------------------------------------------------------------------
const InputComponent& Move::GetInputComponent() const
{
    return m_inputComponent;
}

//----------------------------------------------------------------------------------------------------
U32 Move::GetDirtyState() const
{
    return m_dirtyState;
}

//----------------------------------------------------------------------------------------------------
F32 Move::GetDt() const
{
    return m_dt;
}

//----------------------------------------------------------------------------------------------------
U32 Move::GetFrame() const
{
    return m_frame;
}
}
