#ifndef __OBSERVER_H__
#define __OBSERVER_H__

namespace sand
{

	union Event;

	//----------------------------------------------------------------------------------------------------
	class Observer
	{
	public:
		Observer() {}
		virtual ~Observer() {}

		virtual void OnNotify(const Event& event) = 0;
	};
}

#endif
