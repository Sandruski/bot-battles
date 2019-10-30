#ifndef __TIMER_H__
#define __TIMER_H__

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Timer
	{
	public:
		Timer();
		~Timer();

		void Start();

		F64 ReadSec() const;
		F64 ReadMs() const;

	private:
		U64 m_startCounter;
		U64 m_frequency;
	};
}

#endif
