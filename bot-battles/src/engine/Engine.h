#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <memory>
#include <vector>

#include "Utils.h"

namespace sand
{

	class IModule;
	class ModuleWindow;

	//----------------------------------------------------------------------------------------------------
	struct EngineConfiguration 
	{
		EngineConfiguration() : name(nullptr) {}
		EngineConfiguration(const char* name) : name(name) {}

		const char* name;
	};

	//----------------------------------------------------------------------------------------------------
	class Engine 
	{
	public:
		Engine(const char* name);
		virtual ~Engine();

		bool Init();
		bool Update();
		bool End();

		ModuleWindow& GetWindow() { return *m_window; }

		const char* GetName() const { return m_configuration.name; }

	private:
		void CleanUp();

	private:
		std::vector<IModule*> m_modules;
		EngineConfiguration m_configuration;
		ModuleWindow* m_window;
		bool m_isInitOk;
		bool m_isActive;
	};

	extern Engine* g_engine;
}

#endif
