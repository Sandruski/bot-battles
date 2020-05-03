#ifndef __FSM_H__
#define __FSM_H__

namespace sand {

class State;

//----------------------------------------------------------------------------------------------------
class FSM : public Subject, public Observer {
public:
    FSM();

    void OnNotify(const Event& event) override;

    bool StartUp();
    bool PreUpdate();
    bool Update();
    bool RenderGui();
    bool ShutDown();

    template <class T>
    bool RegisterState();
    template <class T>
    bool DeRegisterState();

    std::weak_ptr<State> GetCurrentState() const;
    template <class T>
    std::weak_ptr<T> GetState() const;

    template <class T>
    bool ChangeState();
    bool ChangeState(std::weak_ptr<State> state);
    bool ChangeState(const std::string& name);

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
    assert(static_cast<U16>(stateType) < INVALID_STATE);
    std::size_t stateIndex = static_cast<std::size_t>(stateType);
    std::shared_ptr<State> state = m_states.at(stateIndex);
    if (state != nullptr) {
        WLOG("State %u is already registered!", stateIndex);
        return false;
    }

    state = std::make_shared<T>();
    state->Create();
    m_states.at(stateIndex) = state;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool FSM::DeRegisterState()
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    StateType stateType = T::GetType();
    assert(static_cast<U16>(stateType) < INVALID_STATE);
    std::size_t stateIndex = static_cast<std::size_t>(stateType);
    std::shared_ptr<State> state = m_states.at(stateIndex);
    if (state == nullptr) {
        WLOG("State %u is not registered!", stateIndex);
        return false;
    }

    state->Destroy();
    state = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> FSM::GetState() const
{
    static_assert(std::is_base_of<State, T>::value, "T is not derived from State");

    StateType stateType = T::GetType();
    assert(static_cast<U16>(stateType) < INVALID_STATE);
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
    assert(static_cast<U16>(stateType) < INVALID_STATE);
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
