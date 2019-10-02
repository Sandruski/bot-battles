#include "Engine.h"
#include "ModuleTextureImporter.h"
#include "ModuleRenderer.h"
#include "ModuleResourceManager.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleFSM.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Engine::Engine(const EngineConfiguration& configuration) :
		m_configuration(configuration), 
		m_isInitOk(false),
		m_isRunning(false)
	{
		m_window = std::make_unique<ModuleWindow>();
		m_input = std::make_unique<ModuleInput>();
		m_renderer = std::make_unique<ModuleRenderer>();
		m_textureImporter = std::make_unique<ModuleTextureImporter>();
		m_resourceManager = std::make_unique<ModuleResourceManager>();
		m_fsm = std::make_unique<ModuleFSM>();
	}

	//----------------------------------------------------------------------------------------------------
	Engine::~Engine() 
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::Init() 
	{
		m_isInitOk = m_window->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		m_isInitOk = m_input->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		m_isInitOk = m_renderer->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		m_isInitOk = m_textureImporter->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		m_isInitOk = m_resourceManager->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		m_isInitOk = m_fsm->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		if (m_configuration.StatesSetup != nullptr)
		{
			m_configuration.StatesSetup();
		}

		m_isRunning = true;

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	// game loop
	bool Engine::Update() 
	{
		if (!m_isInitOk)
		{
			return false;
		}

		bool ret = m_input->m_isActive && m_input->Update();
		if (!ret)
		{
			return false;
		}
#ifdef _RELEASE
		else if (!m_isRunning)
		{
			return true;
		}
#endif

		ret = m_fsm->m_isActive && m_fsm->Update();
		if (!ret)
		{
			return false;
		}

		ret = LateUpdate();
		if (!ret)
		{
			return false;
		}

		ret = Draw();
		if (!ret)
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::End() 
	{
		if (!m_isInitOk)
		{
			return false;
		}

		bool ret = m_fsm->ShutDown();
		if (!ret)
		{
			return false;
		}
			
		m_resourceManager->ShutDown();
		if (!ret)
		{
			return false;
		}

		ret = m_textureImporter->ShutDown();
		if (!ret)
		{
			return false;
		}

		ret = m_renderer->ShutDown();
		if (!ret)
		{
			return false;
		}

		ret = m_input->ShutDown();
		if (!ret)
		{
			return false;
		}

		ret = m_window->ShutDown();
		if (!ret)
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::LateUpdate()
	{
		if (!m_isInitOk)
		{
			return false;
		}

		bool ret = m_fsm->m_isActive && m_fsm->LateUpdate();
		if (!ret)
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::Draw()
	{
		if (!m_isInitOk)
		{
			return false;
		}

		bool ret = m_renderer->m_isActive && m_renderer->Draw();
		if (!ret)
		{
			return false;
		}

		ret = m_fsm->m_isActive && m_fsm->Draw();
		if (!ret)
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	EngineConfiguration::EngineConfiguration() :
		name(nullptr),
		StatesSetup(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	EngineConfiguration::EngineConfiguration(const char* name, std::function<void()> StatesSetup) :
		name(name),
		StatesSetup(StatesSetup)
	{
	}

	//----------------------------------------------------------------------------------------------------
	EngineConfiguration::EngineConfiguration(const EngineConfiguration& configuration) :
		name(configuration.name),
		StatesSetup(configuration.StatesSetup)
	{
	}
}
