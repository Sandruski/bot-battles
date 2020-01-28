#ifndef __INPUT_BUFFER_H__
#define __INPUT_BUFFER_H__

#include "Input.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class InputBuffer {
public:
    InputBuffer();

    const Input& Add(const InputComponent& inputComponent, U32 dirtyState);
    const Input& Add(const Input& input);
    void Remove(U32 frame);
    void Clear();

    const Input& Get(U32 index) const;
    const Input& GetLast() const;
    bool HasInputs() const;
    U32 GetCount() const;

private:
    std::deque<Input> m_inputs;
};
}

#endif
