#include "Subject.h"

#include "Observer.h"
#include "Events.h"

namespace sand {


	//----------------------------------------------------------------------------------------------------
	Subject::Subject()
	{
	}

	//----------------------------------------------------------------------------------------------------
	Subject::~Subject()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool Subject::AddObserver(std::shared_ptr<Observer> observer)
	{
		auto it = std::find(m_observers.begin(), m_observers.end(), observer);
		if (it != m_observers.end()) {

			WLOG("The observer is already registered!");
			return false;
		}

		m_observers.push_back(observer);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool Subject::RemoveObserver(std::shared_ptr<Observer> observer)
	{
		auto it = std::find(m_observers.begin(), m_observers.end(), observer);
		if (it == m_observers.end()) {

			WLOG("The observer is not registered!");
			return false;
		}

		m_observers.erase(it);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void Subject::PushEvent(Event event)
	{
		m_events.push(event);
	}

	//----------------------------------------------------------------------------------------------------
	void Subject::NotifyAll()
	{
		while (!m_events.empty())
		{
			Event event = m_events.front();

			for (const auto& observer : m_observers)
			{
				observer->OnNotify(event);
			}

			m_events.pop();
		}
	}
}
