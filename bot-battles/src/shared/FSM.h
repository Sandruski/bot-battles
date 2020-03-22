#ifndef __FSM_H__
#define __FSM_H__

namespace sand {

class State;

//----------------------------------------------------------------------------------------------------
class FSM : public Observer {
public:
    FSM();

    void OnNotify(const Event& event) override;

    bool StartUp();
    bool Update();
    bool RenderGui();
    bool ShutDown();

    template <class T>
    bool RegisterState();
    template <class T>
    bool DeRegisterState();

    template <class T>
    std::weak_ptr<T> GetState();

    template <class T>
    bool ChangeState();
    bool ChangeState(const char* name);

private:
    bool ChangeState(std::weak_ptr<State> state);

    void OnPlayerAdded();
    void OnPlayerRemoved();

private:
    std::array<std::shared_ptr<State>, MAX_STATES> m_states;
    std::weak_ptr<State> m_currentState;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool FSM::RegisterState()
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    StateType stateType = T::GetType();
    assert(stateType < StateType::COUNT);
    std::size_t stateIndex = static_cast<std::size_t>(stateType);
    std::shared_ptr<State> state = m_states.at(stateIndex);
    if (state != nullptr) {
        WLOG("State %u is already registered!", stateIndex);
        return false;
    }

    m_states.at(stateIndex) = std::make_shared<T>();

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool FSM::DeRegisterState()
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    StateType stateType = T::GetType();
    assert(stateType < StateType::COUNT);
    std::size_t stateIndex = static_cast<std::size_t>(stateType);
    std::shared_ptr<State> state = m_states.at(stateIndex);
    if (state == nullptr) {
        WLOG("State %u is not registered!", stateIndex);
        return false;
    }

    m_states.at(stateIndex) = nullptr;

    return false;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> FSM::GetState()
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    StateType stateType = T::GetType();
    assert(stateType < StateType::COUNT);
    std::size_t stateIndex = static_cast<std::size_t>(stateType);
    std::shared_ptr<State> baseState = m_states.at(stateIndex);
    std::weak_ptr<T> derivedState = std::weak_ptr<T>(std::static_pointer_cast<T>(baseState));

    return derivedState;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool FSM::ChangeState()
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    StateType stateType = T::GetType();
    assert(stateType < StateType::COUNT);
    std::size_t stateIndex = static_cast<std::size_t>(stateType);
    std::shared_ptr<State> state = m_states.at(stateIndex);
    if (state == nullptr) {
        WLOG("State could not be changed to %u!", stateIndex);
        return false;
    }

    ChangeState(std::weak_ptr<State>(state));

    return true;
}
}

#endif
