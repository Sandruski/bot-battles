#include "ModuleFSM.h"

#include "State.h"
#include "Log.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	const char* ModuleFSM::GetName()
	{
		return "FSM";
	}

	//----------------------------------------------------------------------------------------------------
	StateID ModuleFSM::GenerateID()
	{
		static StateID currentID = 0;
		++currentID;
		return currentID;
	}

	//----------------------------------------------------------------------------------------------------
	ModuleFSM::ModuleFSM() : Module(true),
		m_states(),
		m_currentState()
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
			return m_currentState->Update(0.0f); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::LateUpdate()
	{
		if (m_currentState != nullptr)
		{
			return m_currentState->LateUpdate(0.0f); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::Draw()
	{
		if (m_currentState != nullptr)
		{
			return m_currentState->Draw();
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	U64 ModuleFSM::AddState(std::shared_ptr<State> state)
	{
		assert(state != nullptr);

		StateID id = GenerateID();
		auto inserted = m_states.insert(std::make_pair(id, state));
		if (inserted.second)
		{
			inserted.first->second->Create();
		}
		else
		{
			LOG("State could not be inserted");
		}

		return inserted.first->first;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::RemoveState(U64 id)
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
	void ModuleFSM::RemoveAllStates()
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
	bool ModuleFSM::ChangeState(U64 id)
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
