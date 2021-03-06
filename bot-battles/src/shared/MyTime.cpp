#include "MyTime.h"

#include "Game.h"
#include "RendererComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MyTime& MyTime::GetInstance()
{
    static MyTime s_instance;
    return s_instance;
}

//----------------------------------------------------------------------------------------------------
MyTime::MyTime()
    : m_timer()
    , m_dtTimer()
    , m_fpsTrack()
    , m_lastFrameMs(0.0f)
    , m_fps(0.0f)
    , m_dt(0.0f)
    , m_startFrameTime(0.0f)
    , m_frame(0)
{
    m_timer.Start();
    m_fpsTrack.resize(60);
}

//----------------------------------------------------------------------------------------------------
void MyTime::StartUpdate()
{
    m_dtTimer.Start();

    m_startFrameTime = GetTime();
    ++m_frame;
}

//----------------------------------------------------------------------------------------------------
void MyTime::FinishUpdate()
{
    m_lastFrameMs = m_dtTimer.ReadMs();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    if (!rendererComponent.lock()->m_isVSync && windowComponent.lock()->m_isCap) {
        F64 desiredLastFrameMs = 1000.0 / windowComponent.lock()->m_fps;
        if (m_lastFrameMs < desiredLastFrameMs) {
            SDL_Delay(static_cast<U32>(desiredLastFrameMs - m_lastFrameMs));
            m_lastFrameMs = m_dtTimer.ReadMs();
        }
    }

    m_fps = 1000.0 / m_lastFrameMs;
    AddFpsToTrack(static_cast<F32>(m_fps));
    m_dt = 1.0 / m_fps;
}

//----------------------------------------------------------------------------------------------------
const std::vector<F32>& MyTime::GetFpsTrack() const
{
    return m_fpsTrack;
}

//----------------------------------------------------------------------------------------------------
F32 MyTime::GetTime() const
{
    return static_cast<F32>(m_timer.ReadSec());
}

//----------------------------------------------------------------------------------------------------
F32 MyTime::GetStartFrameTime() const
{
    return m_startFrameTime;
}

//----------------------------------------------------------------------------------------------------
F32 MyTime::GetDt() const
{
    return static_cast<F32>(m_dt);
}

//----------------------------------------------------------------------------------------------------
F32 MyTime::GetFps() const
{
    return static_cast<F32>(m_fps);
}

//----------------------------------------------------------------------------------------------------
U32 MyTime::GetFrame() const
{
    return m_frame;
}

//----------------------------------------------------------------------------------------------------
void MyTime::AddFpsToTrack(F32 fps)
{
    for (std::size_t i = m_fpsTrack.size() - 1; i > 0; --i)
        m_fpsTrack.at(i) = m_fpsTrack.at(i - 1);

    m_fpsTrack.at(0) = fps;
}
}
