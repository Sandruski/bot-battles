#include "ModuleFSM.h"

#include "State.h"
#include "Log.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleFSM::ModuleFSM() : Module(true),
		m_states(),
		m_currentState(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleFSM::~ModuleFSM()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::StartUp()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::ShutDown()
	{
		RemoveAllStates();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::Update()
	{
		if (m_currentState != nullptr)
		{
			m_currentState->Update(0.0f); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleFSM::GetName() const
	{
		return "FSM";
	}

	//----------------------------------------------------------------------------------------------------
	std::shared_ptr<State> ModuleFSM::AddState(std::shared_ptr<State> state, bool isCurrent)
	{
		std::shared_ptr<State> newState = state;

		auto ret = m_states.insert(std::pair<U64, std::shared_ptr<State>>(state->GetID(), state));
		if (ret.second)
		{
			LOG("State could not be inserted");
			newState = ret.first->second;
		}

		if (isCurrent)
		{
			m_currentState->Exit();
			m_currentState = newState;
			m_currentState->Enter();
		}

		return state;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::RemoveState(U64 id)
	{
		std::shared_ptr<State> state = GetState(id);
		if (state == nullptr)
		{
			return false;
		}

		if (m_currentState == state)
		{
			m_currentState->Exit();
		}

		m_states.erase(id);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleFSM::RemoveAllStates()
	{
		if (m_currentState != nullptr)
		{
			m_currentState->Exit();
		}

		m_states.clear();
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::ChangeState(U64 id)
	{
		std::shared_ptr<State> newState = GetState(id);
		if (newState == nullptr)
		{
			return false;
		}

		m_currentState->Exit();
		m_currentState = newState;
		m_currentState->Enter();

		return true;

		// TODO: send event ("the state has been changed to blablabla")
	}

	//----------------------------------------------------------------------------------------------------
	std::shared_ptr<State> ModuleFSM::GetState(U64 id) const
	{
		auto it = m_states.find(id);
		if (it != m_states.end())
		{
			return (*it).second;
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------------
	std::shared_ptr<State> ModuleFSM::GetCurrentState() const
	{
		return m_currentState;
	}
}