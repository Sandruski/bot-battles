#include "Time.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	Time& Time::GetInstance()
	{
		static Time s_instance;
		return s_instance;
	}

	//----------------------------------------------------------------------------------------------------
	Time::Time()
		: m_timer(),
		m_lastFrameMs(0.0f),
		m_desiredFramerate(60.0f),
		m_fps(0.0f),
		m_dt(0.0f)
	{
	}

	//----------------------------------------------------------------------------------------------------
	Time::~Time()
	{
	}

	//----------------------------------------------------------------------------------------------------
	void Time::StartUpdate()
	{
		m_timer.Start();
	}

	//----------------------------------------------------------------------------------------------------
	void Time::FinishUpdate()
	{
		m_lastFrameMs = m_timer.ReadMs();
		F64 desiredLastFrameMs = 1000.0 / m_desiredFramerate;
		if (m_lastFrameMs < desiredLastFrameMs) {
			SDL_Delay(static_cast<U32>(desiredLastFrameMs - m_lastFrameMs));
			m_lastFrameMs = m_timer.ReadMs();
		}

		m_fps = 1000.0 / m_lastFrameMs;
		m_dt = 1.0 / m_fps;
	}

	//----------------------------------------------------------------------------------------------------
	F32 Time::GetTime() const
	{
		return static_cast<F32>(m_timer.ReadSec());
	}
}