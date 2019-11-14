#ifndef __MOVE_H__
#define __MOVE_H__

#include "SingletonInputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Move : public WriteNetComponent, public ReadNetComponent {
public:
    Move(const SingletonInputComponent& input, F32 timestamp, F32 dt);
    ~Move();

    const SingletonInputComponent& GetInput() const
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

    void Write(OutputMemoryStream& outputStream, U16 memberFlags) const override;
    void Read(InputMemoryStream& inputStream, U16 memberFlags) override;

private:
    SingletonInputComponent m_input;

    F32 m_timestamp;
    F32 m_dt;
};
}

#endif
