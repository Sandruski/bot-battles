#ifndef __INPUT_BUFFER_H__
#define __INPUT_BUFFER_H__

#include "Input.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class InputBuffer {
public:
    InputBuffer();

    void Add(const InputComponent& inputComponent, U32 dirtyState);
    void Add(const Input& input);
    void Remove(U32 frame);
    void Clear();

    U32 GetIndex(U32 frame) const;
    const Input& GetInput(U32 index) const;
    const Input& GetLastInput() const;

    bool IsEmpty() const;
    bool IsFull() const;
    U32 GetCount() const;

public:
    U32 m_front;
    U32 m_back;

private:
    std::array<Input, MAX_INPUTS> m_inputs;
};
}

#endif
