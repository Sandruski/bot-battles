#ifndef __INPUT_H__
#define __INPUT_H__

#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Input {
public:
    Input();
    Input(const InputComponent& inputComponent, U32 dirtyState, F32 dt, U32 frame);

#ifdef _CLIENT
    void Write(OutputMemoryStream& outputStream) const;
#elif defined(_SERVER)
    void Read(InputMemoryStream& inputStream);
#endif

    const InputComponent& GetInputComponent() const;
    U32 GetDirtyState() const;
    F32 GetDt() const;
    U32 GetFrame() const;

private:
    InputComponent m_inputComponent;
    U32 m_dirtyState;
    F32 m_dt;
    U32 m_frame;
};
}

#endif