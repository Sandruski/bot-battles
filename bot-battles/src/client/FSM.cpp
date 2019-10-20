#include "FSM.h"

#include "State.h"
#include "StateDefs.h"


namespace sand
{

	//----------------------------------------------------------------------------------------------------
	FSM::FSM() :
		m_states(),
		m_availableStates(),
		m_currentState(nullptr)
	{
		m_states.reserve(MAX_STATES);

		for (U32 i = 0; i < MAX_STATES; ++i) {
			m_availableStates.push(i);
		}
	}

	//----------------------------------------------------------------------------------------------------
	FSM::~FSM()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::ShutDown()
	{
		m_states.clear();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::PreUpdate(F32 dt)
	{
		if (m_currentState != nullptr)
		{
			return m_currentState->PreUpdate(dt); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::Update(F32 dt)
	{
		if (m_currentState != nullptr)
		{
			return m_currentState->Update(dt); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::PostUpdate(F32 dt)
	{
		if (m_currentState != nullptr)
		{
			return m_currentState->PostUpdate(dt); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::Render()
	{
		if (m_currentState != nullptr)
		{
			return m_currentState->Render();
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::RemoveState(U32 id)
	{
		auto it = m_states.find(id);
		if (it == m_states.end())
		{
			LOG("State could not be removed");
			return false;
		}

		if ((*it).second == m_currentState)
		{
			m_currentState->Exit();
		}

		(*it).second->Destroy();

		m_states.erase(id);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void FSM::RemoveAllStates()
	{
		if (m_currentState != nullptr)
		{
			m_currentState->Exit();
		}

		for (auto& state : m_states)
		{
			state.second->Destroy();
		}

		m_states.clear();
	}

	//----------------------------------------------------------------------------------------------------
	bool FSM::ChangeState(U32 id)
	{
		auto it = m_states.find(id);
		if (it == m_states.end())
		{
			LOG("State could not be changed");
			return false;
		}

		if (m_currentState != nullptr)
		{
			m_currentState->Exit();
		}

		m_currentState = (*it).second;
		m_currentState->Enter();

		return true;

		// TODO: send event ("the state has been changed to blablabla")
	}
}
