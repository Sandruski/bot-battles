#ifndef __FSM_H__
#define __FSM_H__

namespace sand {

class State;

//----------------------------------------------------------------------------------------------------
class FSM {
public:
    FSM();
    ~FSM();

    bool ShutDown();

    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool Render();

    template <class T>
    U32 AddState();
    bool RemoveState(U32 id);
    void RemoveAllStates();
    bool ChangeState(U32 id);

private:
    std::unordered_map<U32, std::shared_ptr<State>> m_states;
    std::queue<U32> m_availableStates;
    std::shared_ptr<State> m_currentState;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline U32 FSM::AddState()
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    U32 id = m_availableStates.front();
    m_availableStates.pop();

    auto inserted = m_states.insert(std::make_pair(id, std::make_shared<T>()));
    if (inserted.second) {
        inserted.first->second->Create();
    } else {
        ELOG("State could not be inserted");
    }

    return inserted.first->first;
}
}

#endif
