#ifndef __SUBJECT_H__
#define __SUBJECT_H__

namespace sand {

class Observer;

//----------------------------------------------------------------------------------------------------
class Subject {
public:
    Subject();
    virtual ~Subject() = default;

    bool AddObserver(std::weak_ptr<Observer> observer);
    bool RemoveObserver(std::weak_ptr<Observer> observer);

    void PushEvent(Event event);

    void NotifyEvent(Event event) const;
    void NotifyEvents();

private:
    std::vector<std::weak_ptr<Observer>> m_observers;
    std::queue<Event> m_events;
};
}

#endif
