#ifndef __FRAME_H__
#define __FRAME_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Frame {
public:
    Frame();
    Frame(U32 frame, F32 timestamp);

    U32 GetFrame() const;
    F32 GetTimestamp() const;

private:
    U32 m_frame;
    F32 m_timestamp;
};
}

#endif
