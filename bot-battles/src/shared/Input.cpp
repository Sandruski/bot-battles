#include "Input.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Input::Input()
    : m_inputComponent()
    , m_dirtyState(0)
    , m_dt(0.0f)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Input::Input(const InputComponent& inputComponent, U32 dirtyState, F32 dt, U32 frame)
    : m_inputComponent(inputComponent)
    , m_dirtyState(dirtyState)
    , m_dt(dt)
    , m_frame(frame)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void Input::Write(OutputMemoryStream& outputStream) const
{
    outputStream.Write(m_dirtyState);
    m_inputComponent.Write(outputStream, m_dirtyState);
    outputStream.Write(m_dt);
    outputStream.Write(m_frame);
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
void Input::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_dirtyState);
    m_inputComponent.Read(inputStream, m_dirtyState, ReplicationActionType::NONE, INVALID_ENTITY);
    inputStream.Read(m_dt);
    inputStream.Read(m_frame);
}
#endif

//----------------------------------------------------------------------------------------------------
const InputComponent& Input::GetInputComponent() const
{
    return m_inputComponent;
}

//----------------------------------------------------------------------------------------------------
U32 Input::GetDirtyState() const
{
    return m_dirtyState;
}

//----------------------------------------------------------------------------------------------------
F32 Input::GetDt() const
{
    return m_dt;
}

//----------------------------------------------------------------------------------------------------
U32 Input::GetFrame() const
{
    return m_frame;
}
}
