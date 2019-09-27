#include "Engine.h"
#include "ModuleWindow.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	Engine::Engine(const char *name) :
		m_modules(),
		m_configuration(name), 
		m_window(nullptr), 
		m_isInitOk(false),
		m_isActive(false) 
	{
		m_modules.reserve(static_cast<size_t>(ModuleType::MODULE_COUNT));

		m_window = new ModuleWindow();
		m_modules.push_back(static_cast<IModule*>(m_window));
	}

	//----------------------------------------------------------------------------------------------------
	Engine::~Engine() 
	{ 
		CleanUp();
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::Init() 
	{
		m_isInitOk = true;

		for (auto it = m_modules.begin(); it != m_modules.end() && m_isInitOk; ++it) 
		{
			m_isInitOk = (*it)->StartUp();
		}

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::Update() 
	{
		bool ret = m_isInitOk;

		if (ret) 
		{

		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::End() 
	{
		bool ret = m_isInitOk;

		for (auto it = m_modules.rbegin(); it != m_modules.rend() && ret; ++it) 
		{
			ret = (*it)->ShutDown();
		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	void Engine::CleanUp() 
	{
		for (auto it = m_modules.rbegin(); it != m_modules.rend(); ++it) 
		{
			SAFE_DELETE_POINTER(*it);
		}

		m_modules.clear();
	}
}
