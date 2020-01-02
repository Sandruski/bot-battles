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
    F32 GetStartFrameTime() const;
    F32 GetDt() const;
    F32 GetFps() const;

private:
    Timer m_timer;
    Timer m_dtTimer;

    F32 m_startFrameTime;
    F64 m_lastFrameMs;
    F64 m_fps;
    F64 m_dt;
};
}

#endif
