#include "ModuleFSM.h"

#include "State.h"
#include "Log.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleFSM::ModuleFSM() : Module(true),
		m_states(),
		m_currentState(),
		m_id(0)
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
		RemoveAll();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleFSM::GetName() const
	{
		return "FSM";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::Update()
	{
		if (m_currentState.lock() != nullptr)
		{
			return m_currentState.lock()->Update(0.0f); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::LateUpdate()
	{
		if (m_currentState.lock() != nullptr)
		{
			return m_currentState.lock()->LateUpdate(0.0f); // TODO dt
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::Draw()
	{
		if (m_currentState.lock() != nullptr)
		{
			return m_currentState.lock()->Draw();
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	U64 ModuleFSM::Add(std::shared_ptr<State> state)
	{
		assert(state != nullptr);

		auto inserted = m_states.insert(std::make_pair(m_id, state));
		if (inserted.second)
		{
			inserted.first->second->Create();

			++m_id;
		}
		else
		{
			LOG("State could not be inserted");
		}

		return inserted.first->first;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::Remove(U64 id)
	{
		std::shared_ptr<State> state = Get(id);
		if (state == nullptr)
		{
			LOG("State could not be removed");
			return false;
		}

		if (state == m_currentState.lock())
		{
			m_currentState.lock()->Exit();
		}

		state->Destroy();

		m_states.erase(id);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleFSM::RemoveAll()
	{
		if (m_currentState.lock() != nullptr)
		{
			m_currentState.lock()->Exit();
		}

		for (auto& state : m_states)
		{
			state.second->Destroy();
		}

		m_states.clear();
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleFSM::Change(U64 id)
	{
		std::shared_ptr<State> state = Get(id);
		if (state == nullptr)
		{
			LOG("State could not be changed");
			return false;
		}

		if (m_currentState.lock() != nullptr)
		{
			m_currentState.lock()->Exit();
		}

		m_currentState = state;
		m_currentState.lock()->Enter();

		return true;

		// TODO: send event ("the state has been changed to blablabla")
	}

	//----------------------------------------------------------------------------------------------------
	std::shared_ptr<State> ModuleFSM::Get(U64 id) const
	{
		auto it = m_states.find(id);
		if (it != m_states.end())
		{
			return (*it).second;
		}
		
		return nullptr;
	}
}
