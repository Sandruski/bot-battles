#ifndef __I_MODULE_H__
#define __I_MODULE_H__

#include "Utils.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class IModule
	{
	public:
		IModule() {}
		virtual ~IModule() {}

		virtual bool StartUp() = 0;
		virtual bool ShutDown() = 0;

		virtual ModuleType GetType() const = 0;
		virtual bool IsActive() const = 0;
	};
}

#endif
