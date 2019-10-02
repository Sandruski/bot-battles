#ifndef __MODULE_FSM_H__
#define __MODULE_FSM_H__

#include "Module.h"
#include "Memory.h"

#include <memory>
#include <unordered_map>

namespace sand
{

	class State;

	//----------------------------------------------------------------------------------------------------
	class ModuleFSM : public Module
	{
	public:
		ModuleFSM();
		~ModuleFSM();

		bool StartUp() override;
		bool ShutDown() override;

		const char* GetName() const override;

		bool Update();

		std::shared_ptr<State> AddState(std::shared_ptr<State> state, bool isCurrent);
		bool RemoveState(U64 id);
		void RemoveAllStates();

		bool ChangeState(U64 id);

		std::shared_ptr<State> GetState(U64 id) const;
		std::shared_ptr<State> GetCurrentState() const;

	private:
		std::unordered_map<U64, std::shared_ptr<State>> m_states;
		std::shared_ptr<State> m_currentState;
	};
}

#endif