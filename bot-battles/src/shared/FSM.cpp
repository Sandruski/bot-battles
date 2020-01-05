#include "FSM.h"

#include "State.h"
#include "StateDefs.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
FSM::FSM()
    : m_states()
    , m_currentState()
{
    m_states.fill(nullptr);
}

//----------------------------------------------------------------------------------------------------
bool FSM::PreUpdate()
{
    if (!m_currentState.expired()) {
        return m_currentState.lock()->PreUpdate();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool FSM::Update()
{
    if (!m_currentState.expired()) {
        return m_currentState.lock()->Update();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool FSM::PostUpdate()
{
    if (!m_currentState.expired()) {
        return m_currentState.lock()->PostUpdate();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool FSM::ChangeState(const char* name)
{
    for (const auto& state : m_states) {
        if (COMPARE_STRINGS(state->GetName(), name)) {
            if (!m_currentState.expired()) {
                m_currentState.lock()->Exit();
            }

            m_currentState = std::weak_ptr(state);
            m_currentState.lock()->Enter();
            return true;
        }
    }

    WLOG("State could not be changed to %u!", stateIndex);
    return false;
}
}
