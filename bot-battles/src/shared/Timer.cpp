#include "Timer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Timer::Timer()
    : m_startCounter(0)
    , m_frequency(SDL_GetPerformanceFrequency())
{
}

//----------------------------------------------------------------------------------------------------
void Timer::Start()
{
    m_startCounter = SDL_GetPerformanceCounter();
}

//----------------------------------------------------------------------------------------------------
F64 Timer::ReadSec() const
{
    return static_cast<F64>(SDL_GetPerformanceCounter() - m_startCounter) / static_cast<F64>(m_frequency);
}

//----------------------------------------------------------------------------------------------------
F64 Timer::ReadMs() const
{
    return 1000.0 * ReadSec();
}
}
