#ifndef __MODULE_H__
#define __MODULE_H__

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Module
	{
	public:
		Module(bool isActive) : m_isActive(isActive) {}
		virtual ~Module() {}

		virtual bool StartUp() = 0;
		virtual bool ShutDown() = 0;

		virtual const char* GetName() const = 0;

	public:
		bool m_isActive;
	};
}

#endif
