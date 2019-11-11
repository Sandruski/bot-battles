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
    ~Time();

    void StartUpdate();
    void FinishUpdate();

    F32 GetDt() const
    {
        return static_cast<F32>(m_dt);
    }

    F32 GetTime() const;

private:
    Timer m_timer;
    Timer m_dtTimer;

    F64 m_lastFrameMs;
    F64 m_desiredFramerate;
    F64 m_fps;
    F64 m_dt;
};
}

#endif
