#include "Engine.h"
#include "ModuleTextureImporter.h"
#include "ModuleRenderer.h"
#include "ModuleResourceManager.h"
#include "ModuleWindow.h"
#include "ResourceTexture.h"
#include "ModuleInput.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Engine::Engine(const char* name) :
		m_configuration(name),
		m_window(nullptr), 
		m_isInitOk(false),
		m_exit(false)
	{
		m_window = std::make_unique<ModuleWindow>();
		m_input = std::make_unique<ModuleInput>();
		m_renderer = std::make_unique<ModuleRenderer>();
		m_textureImporter = std::make_unique<ModuleTextureImporter>();
		m_resourceManager = std::make_unique<ResourceManager>();
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

		m_resourceTexture = g_engine->GetResourceManager().Add<ResourceTexture>("baker_house.png", "../../data/textures/");

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

		bool ret = m_input->Update();
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

		bool ret = m_resourceManager->ShutDown();
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

		bool ret = m_renderer->Draw();
		if (!ret)
		{
			return false;
		}

		return true;
	}
}
