#ifndef __MOVE_H__
#define __MOVE_H__

#include "InputComponent.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class Move {
public:
    Move(const InputComponent& input, F32 timestamp, F32 dt);
    ~Move();

    const InputComponent& GetInput() const
    {
        return m_input;
    }

    F32 GetTimestamp() const
    {
        return m_timestamp;
    }

    F32 GetDt() const
    {
        return m_dt;
    }

    void Write(OutputMemoryStream& outputStream, U16 memberFlags) const;
    void Read(InputMemoryStream& inputStream);

private:
    InputComponent m_input;

    F32 m_timestamp;
    F32 m_dt;
};
}

#endif
