#include "Game.h"

#include "Engine.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Game::Game() : Module(true),
		m_resourceTexture(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	Game::~Game()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::StartUp()
	{
		m_resourceTexture = g_engine->GetResourceManager().Add<ResourceTexture>("baker_house.png", "../../data/textures/");

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::ShutDown()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	const char* Game::GetName() const
	{
		return "Game";
	}

	//----------------------------------------------------------------------------------------------------
	bool Game::Update()
	{
		return true;
	}
}