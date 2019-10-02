#include "Engine.h"
#include "ModuleTextureImporter.h"
#include "ModuleRenderer.h"
#include "ModuleResourceManager.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleFSM.h"

#include "Game.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Engine::Engine(const char* name) :
		m_configuration(name),
		m_window(nullptr), 
		m_isInitOk(false),
		m_isRunning(false)
	{
		m_window = std::make_unique<ModuleWindow>();
		m_input = std::make_unique<ModuleInput>();
		m_renderer = std::make_unique<ModuleRenderer>();
		m_textureImporter = std::make_unique<ModuleTextureImporter>();
		m_resourceManager = std::make_unique<ModuleResourceManager>();
		m_FSM = std::make_unique<ModuleFSM>();

		m_game = std::make_unique<Game>();
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

		m_isInitOk = m_FSM->StartUp();
		if (!m_isInitOk)
		{
			return false;
		}

		m_game->StartUp();

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

		ret = m_FSM->m_isActive && m_FSM->Update();
		if (!ret)
		{
			return false;
		}

		m_game->Update();

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

		m_game->ShutDown();

		bool ret = m_FSM->ShutDown();
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
		return m_isInitOk;
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

		return true;
	}
}
