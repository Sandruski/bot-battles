#ifndef __MOVE_H__
#define __MOVE_H__

#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Move {
public:
    Move();
    Move(const InputComponent& input, U32 dirtyState, F32 timestamp, F32 dt);

#ifdef _CLIENT
    void Write(OutputMemoryStream& outputStream) const;
#elif defined(_SERVER)
    void Read(InputMemoryStream& inputStream);
#endif

    const InputComponent& GetInput() const;
    U32 GetDirtyState() const;
    F32 GetTimestamp() const;
    F32 GetDt() const;

private:
    InputComponent m_input;
    U32 m_dirtyState;
    F32 m_timestamp;
    F32 m_dt;
};
}

#endif
