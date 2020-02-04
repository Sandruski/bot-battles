#include "Input.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Input::Input()
    : m_inputComponent()
    , m_dirtyState(0)
    , m_dt(0.0f)
    , m_frame(0)
    , m_fromPositionFrame(0)
    , m_toPositionFrame(0)
    , m_interpolation(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Input::Input(const InputComponent& inputComponent, U32 dirtyState, F32 dt, U32 frame, U32 fromPositionFrame, U32 toPositionFrame, F32 interpolation)
    : m_inputComponent(inputComponent)
    , m_dirtyState(dirtyState)
    , m_dt(dt)
    , m_frame(frame)
    , m_fromPositionFrame(fromPositionFrame)
    , m_toPositionFrame(toPositionFrame)
    , m_interpolation(interpolation)
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
    outputStream.Write(m_fromPositionFrame);
    outputStream.Write(m_toPositionFrame);
    outputStream.Write(m_interpolation);
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
void Input::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_dirtyState);
    m_inputComponent.Read(inputStream, m_dirtyState, ReplicationActionType::NONE, INVALID_ENTITY);
    inputStream.Read(m_dt);
    inputStream.Read(m_frame);
    inputStream.Read(m_fromPositionFrame);
    inputStream.Read(m_toPositionFrame);
    inputStream.Read(m_interpolation);
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

//----------------------------------------------------------------------------------------------------
U32 Input::GetFromPositionFrame() const
{
    return m_fromPositionFrame;
}

//----------------------------------------------------------------------------------------------------
U32 Input::GetToPositionFrame() const
{
    return m_toPositionFrame;
}

//----------------------------------------------------------------------------------------------------
F32 Input::GetInterpolation() const
{
    return m_interpolation;
}
}
