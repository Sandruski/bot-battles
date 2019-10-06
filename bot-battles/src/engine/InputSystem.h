#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__

#include "System.h"

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class InputSystem : public System
	{
	public:
		static SystemType GetType() { return SystemType::RENDERER; }

	public:
		InputSystem();
		~InputSystem() override;

		bool StartUp() override;
		bool Update(F32 dt) override;
		bool ShutDown() override;
	};
}

#endif