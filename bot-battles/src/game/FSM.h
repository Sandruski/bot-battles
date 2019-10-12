#ifndef __FSM_H__
#define __FSM_H__

#include "Memory.h"

#include <memory>
#include <unordered_map>
#include <queue>

namespace sand
{

	class State;

	//----------------------------------------------------------------------------------------------------
	class FSM
	{
	public:
		FSM();
		~FSM();

		bool ShutDown();

		bool Update();
		bool LateUpdate();
		bool Draw();

		U32 AddState(std::shared_ptr<State> state);
		bool RemoveState(U32 id);
		void RemoveAllStates();
		bool ChangeState(U32 id);

	private:
		std::unordered_map<U32, std::shared_ptr<State>> m_states;
		std::queue<U32> m_availableStates;
		std::shared_ptr<State> m_currentState;
	};
}

#endif
