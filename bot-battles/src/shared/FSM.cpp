#include "FSM.h"

#include "Config.h"
#include "Game.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
FSM::FSM()
    : m_states()
    , m_currentState()
{
    m_states.fill(nullptr);
}

//----------------------------------------------------------------------------------------------------
void FSM::OnNotify(const Event& event)
{
    if (!m_currentState.expired()) {
        return m_currentState.lock()->OnNotify(event);
    }
}

//----------------------------------------------------------------------------------------------------
bool FSM::StartUp()
{
    return ChangeState("Main Menu");
}

//----------------------------------------------------------------------------------------------------
bool FSM::PreUpdate()
{
    NotifyEvents();

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
bool FSM::Render()
{
    if (!m_currentState.expired()) {
        return m_currentState.lock()->Render();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool FSM::RenderGui()
{
    if (!m_currentState.expired()) {
        return m_currentState.lock()->RenderGui();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool FSM::ShutDown()
{
    ChangeState(std::weak_ptr<State>());

    return true;
}

//----------------------------------------------------------------------------------------------------
std::weak_ptr<State> FSM::GetCurrentState() const
{
    return m_currentState;
}

//----------------------------------------------------------------------------------------------------
bool FSM::ChangeState(std::weak_ptr<State> state)
{
    if (!m_currentState.expired()) {
        m_currentState.lock()->Exit();
    }

    m_currentState = state;

    if (state.expired()) {
        return false;
    }

    m_currentState.lock()->Enter();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool FSM::ChangeState(const std::string& name)
{
    for (const auto& state : m_states) {

        if (state != nullptr && state->GetName() == name) {

            ChangeState(std::weak_ptr<State>(state));

            return true;
        }
    }

    WLOG("State could not be changed to %s", name.c_str());

    return false;
}
}
