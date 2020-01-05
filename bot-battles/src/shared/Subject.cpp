#include "Subject.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Subject::Subject()
    : m_observers()
    , m_events()
{
}

//----------------------------------------------------------------------------------------------------
bool Subject::AddObserver(std::weak_ptr<Observer> observer)
{
    auto it = std::find_if(m_observers.begin(), m_observers.end(), [observer](const auto& value) {
        return observer.lock() == value.lock();
    });
    if (it != m_observers.end()) {
        WLOG("Observer is already registered!");
        return false;
    }

    m_observers.emplace_back(observer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool Subject::RemoveObserver(std::weak_ptr<Observer> observer)
{
    auto it = std::find_if(m_observers.begin(), m_observers.end(), [observer](const auto& value) {
        return observer.lock() == value.lock();
    });
    if (it == m_observers.end()) {
        WLOG("Observer is not registered!");
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
void Subject::NotifyEvent(Event event) const
{
    for (const auto& observer : m_observers) {
        observer.lock()->OnNotify(event);
    }
}

//----------------------------------------------------------------------------------------------------
void Subject::NotifyEvents()
{
    while (!m_events.empty()) {

        Event event = m_events.front();
        for (const auto& observer : m_observers) {
            observer.lock()->OnNotify(event);
        }

        m_events.pop();
    }
}
}
