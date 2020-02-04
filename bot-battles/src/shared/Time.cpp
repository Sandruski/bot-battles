#include "Time.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Time& Time::GetInstance()
{
    static Time s_instance;
    return s_instance;
}

//----------------------------------------------------------------------------------------------------
Time::Time()
    : m_timer()
    , m_dtTimer()
    , m_lastFrameMs(0.0f)
    , m_fps(0.0f)
    , m_dt(0.0f)
    , m_startFrameTime(0.0f)
    , m_frame(0)
{
    m_timer.Start();
}

//----------------------------------------------------------------------------------------------------
void Time::StartUpdate()
{
    m_dtTimer.Start();

    m_startFrameTime = GetTime();
    ++m_frame;
}

//----------------------------------------------------------------------------------------------------
void Time::FinishUpdate()
{
    m_lastFrameMs = m_dtTimer.ReadMs();
    F64 desiredLastFrameMs = 1000.0 / FPS;
    if (m_lastFrameMs < desiredLastFrameMs) {
        SDL_Delay(static_cast<U32>(desiredLastFrameMs - m_lastFrameMs));
        m_lastFrameMs = m_dtTimer.ReadMs();
    }

    m_fps = 1000.0 / m_lastFrameMs;
    m_dt = 1.0 / m_fps;
}

//----------------------------------------------------------------------------------------------------
F32 Time::GetTime() const
{
    return static_cast<F32>(m_timer.ReadSec());
}

//----------------------------------------------------------------------------------------------------
F32 Time::GetStartFrameTime() const
{
    return m_startFrameTime;
}

//----------------------------------------------------------------------------------------------------
F32 Time::GetDt() const
{
    return static_cast<F32>(m_dt);
}

//----------------------------------------------------------------------------------------------------
F32 Time::GetFps() const
{
    return static_cast<F32>(m_fps);
}

//----------------------------------------------------------------------------------------------------
U32 Time::GetFrame() const
{
    return m_frame;
}
}
