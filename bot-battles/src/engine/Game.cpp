#include "Game.h"
#include "ModuleTextureImporter.h"
#include "ModuleResourceManager.h"
#include "ModuleWindow.h"
#include "ModuleFSM.h"

#include "EntityManager.h"
#include "ComponentManager.h"

#include "RendererComponent.h"
#include "InputComponent.h"

#include "RendererSystem.h"
#include "InputSystem.h"

#include "SystemDefs.h"

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
		m_textureImporter = std::make_unique<ModuleTextureImporter>();
		m_resourceManager = std::make_unique<ModuleResourceManager>();
		m_fsm = std::make_unique<ModuleFSM>();

		m_entityManager = std::make_unique<EntityManager>();
		m_componentManager = std::make_unique<ComponentManager>();

		m_rendererComponent = std::make_unique<RendererComponent>(0);
		m_inputComponent = std::make_unique<InputComponent>(0);

		m_systems.reserve(MAX_SYSTEMS);
		m_systems.push_back(std::make_unique<RendererSystem>());
		m_systems.push_back(std::make_unique<InputSystem>());
	}

	//----------------------------------------------------------------------------------------------------
	Game::~Game()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::Init()
	{
		bool ret = true;
		for (U32 i = 0; i < m_systems.size() && ret; ++i)
		{
			ret = m_systems[i]->StartUp();
		}

		m_isInitOk = m_window->StartUp();
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
		bool ret = true;
		for (U32 i = 0; i < m_systems.size() && ret; ++i)
		{
			ret = m_systems[i]->Update(12); // TODO DT
		}
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

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::End()
	{
		bool ret = true;
		for (U32 i = m_systems.size() - 1; i > 0 && ret; --i)
		{
			ret = m_systems[i]->ShutDown();
		}

		ret = m_fsm->ShutDown();
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

		ret = m_window->ShutDown();
		if (!ret)
		{
			return false;
		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::LateUpdate()
	{
		bool ret = true;
		for (U32 i = 0; i < m_systems.size() && ret; ++i)
		{
			ret = m_systems[i]->LateUpdate(12); // TODO DT
		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::Render()
	{
		bool ret = true;
		for (U32 i = 0; i < m_systems.size() && ret; ++i)
		{
			ret = m_systems[i]->Render();
		}

		return ret;
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
