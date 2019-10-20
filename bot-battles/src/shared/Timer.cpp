#include "Timer.h"


namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Timer::Timer() :
		m_startCounter(0),
		m_frequency(0)
	{
		m_frequency = SDL_GetPerformanceFrequency();
	}

	//----------------------------------------------------------------------------------------------------
	Timer::~Timer()
	{
	}

	//----------------------------------------------------------------------------------------------------
	void Timer::Start()
	{
		m_startCounter = SDL_GetPerformanceCounter();
	}

	//----------------------------------------------------------------------------------------------------
	F64 Timer::ReadMs()
	{
		return 1000.0 * static_cast<F64>(SDL_GetPerformanceCounter() - m_startCounter) / static_cast<F64>(m_frequency);
	}
}
