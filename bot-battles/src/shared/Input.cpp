#include "Input.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Input::Input()
    : m_inputComponent()
    , m_dirtyState(0)
    , m_frame(0)
    , m_interpolationFromFrame(0)
    , m_interpolationToFrame(0)
    , m_interpolationPercentage(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Input::Input(const InputComponent& inputComponent, U64 dirtyState, U32 frame, U32 from, U32 to, F32 percentage)
    : m_inputComponent(inputComponent)
    , m_dirtyState(dirtyState)
    , m_frame(frame)
    , m_interpolationFromFrame(from)
    , m_interpolationToFrame(to)
    , m_interpolationPercentage(percentage)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void Input::Write(OutputMemoryStream& outputStream) const
{
    outputStream.Write(m_dirtyState);
    m_inputComponent.Write(outputStream, m_dirtyState);
    outputStream.Write(m_frame);
    outputStream.Write(m_interpolationFromFrame);
    outputStream.Write(m_interpolationToFrame);
    outputStream.Write(m_interpolationPercentage);
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
void Input::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_dirtyState);
    m_inputComponent.Read(inputStream, m_dirtyState, 0, ReplicationActionType::NONE, INVALID_ENTITY);
    inputStream.Read(m_frame);
    inputStream.Read(m_interpolationFromFrame);
    inputStream.Read(m_interpolationToFrame);
    inputStream.Read(m_interpolationPercentage);
}
#endif

//----------------------------------------------------------------------------------------------------
const InputComponent& Input::GetInputComponent() const
{
    return m_inputComponent;
}

//----------------------------------------------------------------------------------------------------
U64 Input::GetDirtyState() const
{
    return m_dirtyState;
}

//----------------------------------------------------------------------------------------------------
U32 Input::GetFrame() const
{
    return m_frame;
}
}
