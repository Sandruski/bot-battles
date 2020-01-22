#ifndef __TIME_H__
#define __TIME_H__

#include "Timer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Time {
public:
    static Time& GetInstance();

public:
    Time();

    void StartUpdate();
    void FinishUpdate();

    F32 GetTime() const;
    F32 GetDt() const;
    F32 GetFps() const;
    U32 GetFrame() const;

private:
    Timer m_timer;
    Timer m_dtTimer;

    F64 m_lastFrameMs;
    F64 m_fps;
    F64 m_dt;
    U32 m_frame;
};
}

#endif
