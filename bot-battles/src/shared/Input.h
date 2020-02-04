#ifndef __INPUT_H__
#define __INPUT_H__

#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Input {
public:
    Input();
    Input(const InputComponent& inputComponent, U32 dirtyState, F32 dt, U32 frame, U32 fromPositionFrame, U32 toPositionFrame, F32 interpolation);

#ifdef _CLIENT
    void Write(OutputMemoryStream& outputStream) const;
#elif defined(_SERVER)
    void Read(InputMemoryStream& inputStream);
#endif

    const InputComponent& GetInputComponent() const;
    U32 GetDirtyState() const;
    F32 GetDt() const;
    U32 GetFrame() const;
    U32 GetFromPositionFrame() const;
    U32 GetToPositionFrame() const;
    F32 GetInterpolation() const;

private:
    InputComponent m_inputComponent;
    U32 m_dirtyState;
    F32 m_dt;
    U32 m_frame;
    U32 m_fromPositionFrame;
    U32 m_toPositionFrame;
    F32 m_interpolation;
};
}

#endif
