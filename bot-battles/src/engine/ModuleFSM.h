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
		~ModuleFSM() override;

		bool StartUp() override;
		bool ShutDown() override;

		const char* GetName() const override;

		bool Update();
		bool LateUpdate();
		bool Draw();

		U64 Add(std::shared_ptr<State> state);
		bool Remove(U64 id);
		void RemoveAll();
		bool Change(U64 id);

	private:
		std::shared_ptr<State> Get(U64 id) const;

	private:
		std::unordered_map<U64, std::shared_ptr<State>> m_states;
		std::weak_ptr<State> m_currentState;
		U64 m_id;
	};
}

#endif