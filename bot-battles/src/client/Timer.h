#ifndef __TIMER_H__
#define __TIMER_H__

#include "Memory.h"

namespace sand
{

	class Timer
	{
	public:
		Timer();
		~Timer();

		void Start();

		F64 ReadMs();

	private:
		U64 m_startCounter;
		U64 m_frequency;
	};
}

#endif
