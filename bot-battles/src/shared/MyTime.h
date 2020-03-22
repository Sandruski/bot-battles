#ifndef __MY_TIME_H__
#define __MY_TIME_H__

#include "MyTimer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MyTime {
public:
    static MyTime& GetInstance();

public:
    MyTime();

    void StartUpdate();
    void FinishUpdate();

    F32 GetTime() const;
    F32 GetStartFrameTime() const;
    F32 GetDt() const;
    F32 GetFps() const;
    U32 GetFrame() const;

private:
    MyTimer m_timer;
    MyTimer m_dtTimer;

    F64 m_lastFrameMs;
    F64 m_fps;
    F64 m_dt;
    F32 m_startFrameTime;
    U32 m_frame;
};
}

#endif
