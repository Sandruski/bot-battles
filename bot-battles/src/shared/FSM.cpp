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
bool FSM::StartUp()
{
    return ChangeState(g_game->GetConfig().m_initialSceneName.c_str());
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

            ChangeState(std::weak_ptr<State>(state));

            return true;
        }
    }

    WLOG("State could not be changed to %s!", name);
    return false;
}

//----------------------------------------------------------------------------------------------------
void FSM::ChangeState(std::weak_ptr<State> state)
{
    if (!m_currentState.expired()) {
        m_currentState.lock()->Exit();
    }

    m_currentState = state;
    m_currentState.lock()->Enter();
}
}
