#include "Game.h"
#include "ModuleTextureImporter.h"
#include "ModuleResourceManager.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleFSM.h"

#include "EntityManager.h"
#include "ComponentManager.h"

#include "RendererSystem.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Game::Game(const GameConfiguration& configuration) :
		m_configuration(configuration), 
		m_isInitOk(false),
		m_isRunning(false)
	{
		m_window = std::make_unique<ModuleWindow>();
		m_input = std::make_unique<ModuleInput>();
		m_textureImporter = std::make_unique<ModuleTextureImporter>();
		m_resourceManager = std::make_unique<ModuleResourceManager>();
		m_fsm = std::make_unique<ModuleFSM>();

		m_entityManager = std::make_unique<EntityManager>();
		m_componentManager = std::make_unique<ComponentManager>();

		m_renderer = std::make_unique<RendererSystem>();
	}

	//----------------------------------------------------------------------------------------------------
	Game::~Game()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::Init()
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
	bool Game::Update()
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

		ret = Render();
		if (!ret)
		{
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::End()
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
	bool Game::LateUpdate()
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
	bool Game::Render()
	{
		if (!m_isInitOk)
		{
			return false;
		}

		bool ret = m_renderer->Render();
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
	GameConfiguration::GameConfiguration() :
		name(nullptr),
		StatesSetup(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	GameConfiguration::GameConfiguration(const char* name, std::function<void()> StatesSetup) :
		name(name),
		StatesSetup(StatesSetup)
	{
	}

	//----------------------------------------------------------------------------------------------------
	GameConfiguration::GameConfiguration(const GameConfiguration& configuration) :
		name(configuration.name),
		StatesSetup(configuration.StatesSetup)
	{
	}
}
