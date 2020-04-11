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

    void Init();
    void StartUpdate();
    void FinishUpdate();

    const std::vector<F32>& GetFpsTrack() const;
    F32 GetTime() const;
    F32 GetStartFrameTime() const;
    F32 GetDt() const;
    F32 GetFps() const;
    U32 GetFrame() const;

private:
    void AddFpsToTrack(F32 fps);

private:
    MyTimer m_timer;
    MyTimer m_dtTimer;

    std::vector<F32> m_fpsTrack;

    F64 m_lastFrameMs;
    F64 m_fps;
    F64 m_dt;
    F32 m_startFrameTime;
    U32 m_frame;
};
}

#endif
