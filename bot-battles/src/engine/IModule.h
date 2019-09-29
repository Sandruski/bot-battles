#ifndef __IMODULE_H__
#define __IMODULE_H__

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class IModule
	{
	public:
		IModule() {}
		virtual ~IModule() {}

		virtual const char* GetName() const = 0;

		virtual bool StartUp() = 0;
		virtual bool ShutDown() = 0;
	};
}

#endif
