#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "IModule.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleInput : public IModule
	{
	public:
		ModuleInput();
		~ModuleInput();

		const char* GetName() const override;

		bool StartUp() override;
		bool ShutDown() override;

		bool Update();

	private:
		bool m_isInitOk;
	};
}

#endif