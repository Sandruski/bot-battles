#ifndef __MODULE_FSM_H__
#define __MODULE_FSM_H__

#include "Module.h"
#include "Memory.h"

#include <memory>
#include <unordered_map>

namespace sand
{

	using StateID = U64;

	class State;

	//----------------------------------------------------------------------------------------------------
	class ModuleFSM : public Module
	{
	public:
		static const char* GetName();
		static StateID GenerateID();

	public:
		ModuleFSM();
		~ModuleFSM() override;

		bool StartUp() override;
		bool ShutDown() override;

		bool Update();
		bool LateUpdate();
		bool Draw();

		StateID AddState(std::shared_ptr<State> state);
		bool RemoveState(U64 id);
		void RemoveAllStates();
		bool ChangeState(U64 id);

	private:
		std::unordered_map<U64, std::shared_ptr<State>> m_states;
		std::shared_ptr<State> m_currentState;
	};
}

#endif