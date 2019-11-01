#ifndef __MOVE_H__
#define __MOVE_H__

#include "InputState.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Move {
public:
    Move(const InputState& inputState, F32 timestamp, F32 dt);
    ~Move();

    const InputState& GetInputState() const
    {
        return m_inputState;
    }

    F32 GetTimestamp() const
    {
        return m_timestamp;
    }

    F32 GetDt() const
    {
        return m_dt;
    }

    bool Write(OutputMemoryStream& outputStream) const;
    bool Read(InputMemoryStream& inputStream);

private:
    InputState m_inputState;

    F32 m_timestamp;
    F32 m_dt;
};
}

#endif
