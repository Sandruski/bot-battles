#ifndef __MOVE_H__
#define __MOVE_H__

#include "InputComponent.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class Move {
public:
    Move();
    Move(const InputComponent& input, U32 dirtyState, F32 timestamp, F32 dt);
    ~Move();

    const InputComponent& GetInput() const
    {
        return m_input;
    }

    U32 GetDirtyState() const
    {
        return m_dirtyState;
    }

    F32 GetTimestamp() const
    {
        return m_timestamp;
    }

    F32 GetDt() const
    {
        return m_dt;
    }

    void Write(OutputMemoryStream& outputStream) const;
    void Read(InputMemoryStream& inputStream);

private:
    InputComponent m_input;
    U32 m_dirtyState;
    F32 m_timestamp;
    F32 m_dt;
};
}

#endif
