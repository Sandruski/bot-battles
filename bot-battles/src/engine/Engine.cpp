#include "Engine.h"
#include "ModuleTextureImporter.h"
#include "ModuleRenderer.h"
#include "ModuleResourceManager.h"
#include "ModuleWindow.h"
#include "ResourceTexture.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	Engine::Engine(const char* name) :
		m_configuration(name),
		m_window(nullptr), 
		m_isInitOk(false),
		m_isActive(false) 
	{
		m_window = std::make_unique<ModuleWindow>();
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
		m_window->StartUp();
		m_renderer->StartUp();
		m_textureImporter->StartUp();
		m_resourceManager->StartUp();

		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	// game loop
	bool Engine::Update() 
	{
		bool ret = m_isInitOk;

		if (ret) 
		{
			ret = PreUpdate();

			if (ret)
			{

				ret = PostUpdate();
			}

		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::End() 
	{
		bool ret = m_isInitOk;

		if (ret)
		{
			m_resourceManager->ShutDown();
			m_textureImporter->ShutDown();
			m_renderer->ShutDown();
			m_window->ShutDown();
		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::PreUpdate()
	{
		bool ret = m_isInitOk;

		if (ret)
		{
			ret = m_renderer->PreUpdate();
		}

		return ret;
	}

	//----------------------------------------------------------------------------------------------------
	bool Engine::PostUpdate()
	{
		bool ret = m_isInitOk;

		if (ret)
		{
			ret = m_renderer->PostUpdate();
		}

		return ret;
	}
}
