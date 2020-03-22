#include "MyTimer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MyTimer::MyTimer()
    : m_startCounter(0)
    , m_frequency(SDL_GetPerformanceFrequency())
{
}

//----------------------------------------------------------------------------------------------------
void MyTimer::Start()
{
    m_startCounter = SDL_GetPerformanceCounter();
}

//----------------------------------------------------------------------------------------------------
F64 MyTimer::ReadSec() const
{
    return static_cast<F64>(SDL_GetPerformanceCounter() - m_startCounter) / static_cast<F64>(m_frequency);
}

//----------------------------------------------------------------------------------------------------
F64 MyTimer::ReadMs() const
{
    return 1000.0 * ReadSec();
}
}
