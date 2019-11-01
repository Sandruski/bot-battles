#ifndef __INPUT_STATE_H__
#define __INPUT_STATE_H__

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class InputState {
public:
    InputState();
    ~InputState();

    F32 GetDesiredHorizontalDelta() const;
    F32 GetDesiredVerticalDelta() const;

    bool IsShooting() const
    {
        return m_isShooting;
    }

    bool Write(OutputMemoryStream& outputStream) const;
    bool Read(InputMemoryStream& inputStream);

private:
    F32 m_desiredRightAmount;
    F32 m_desiredLeftAmount;
    F32 m_desiredForwardAmount;
    F32 m_desiredBackAmount;
    bool m_isShooting;
};
}

#endif
