#ifndef __STATE_H__
#define __STATE_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class State
	{
	public:
		State(U64 id) : m_id(id) {}
		virtual ~State() {}

		virtual void Enter() = 0;
		virtual void Update(F32 dt) {}
		virtual void Render() {}
		virtual void Exit() = 0;

		U64 GetID() const { return m_id; }

	private:
		U64 m_id;
	};
}

#endif