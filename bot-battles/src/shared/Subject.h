#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#include "Events.h"


namespace sand
{

	class Observer;

	//----------------------------------------------------------------------------------------------------
	class Subject
	{
	public:
		Subject();
		virtual ~Subject();

		bool AddObserver(std::shared_ptr<Observer> observer);
		bool RemoveObserver(std::shared_ptr<Observer> observer);

		void PushEvent(Event event);

		void NotifyAll();

	private:
		std::vector<std::shared_ptr<Observer>> m_observers;
		std::queue<Event> m_events;
	};
}

#endif
