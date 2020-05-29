#ifndef __INPUT_H__
#define __INPUT_H__

#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Input {
public:
    Input();
    Input(const InputComponent& inputComponent, U64 dirtyState, U32 frame, U32 from, U32 to, F32 percentage);

#ifdef _CLIENT
    void Write(OutputMemoryStream& outputStream) const;
#elif defined(_SERVER)
    void Read(InputMemoryStream& inputStream);
#endif

    const InputComponent& GetInputComponent() const;
    U64 GetDirtyState() const;
    U32 GetFrame() const;

public:
    U32 m_interpolationFromFrame;
    U32 m_interpolationToFrame;
    F32 m_interpolationPercentage;

private:
    InputComponent m_inputComponent;
    U64 m_dirtyState;
    U32 m_frame;
};
}

#endif
