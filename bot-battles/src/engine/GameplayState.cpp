#include "GameplayState.h"

#include "Engine.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"
#include "ModuleFSM.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	GameplayState::GameplayState() :
		m_resourceTexture(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	GameplayState::~GameplayState()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::Create()
	{
		m_resourceTexture = g_engine->GetResourceManager().Add<ResourceTexture>("baker_house.png", "../../data/textures/");

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::Destroy()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::Enter()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::Update(F32 /*dt*/)
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::LateUpdate(F32 /*dt*/)
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::Draw()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool GameplayState::Exit()
	{
		return true;
	}
}