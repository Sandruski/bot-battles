#include "GameplayState.h"

#include "Game.h"
#include "ModuleResourceManager.h"
#include "ResourceTexture.h"
#include "ModuleFSM.h"

#include "EntityManager.h"
#include "ComponentManager.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"

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
		m_resourceTexture = g_game->GetResourceManager().AddResource<ResourceTexture>("baker_house.png", "../../data/textures/");
		
		EntityID entityID = g_game->GetEntityManager().AddEntity();
		EntityID entityID2 = g_game->GetEntityManager().AddEntity();
		ComponentID transformID = g_game->GetComponentManager().AddComponent<TransformComponent>(entityID);
		ComponentID transformID2 = g_game->GetComponentManager().AddComponent<TransformComponent>(entityID2);
		ComponentID spriteID = g_game->GetComponentManager().AddComponent<SpriteComponent>(entityID);
		ComponentID spriteID2 = g_game->GetComponentManager().AddComponent<SpriteComponent>(entityID2);

		bool removed = g_game->GetComponentManager().RemoveComponent<TransformComponent>(entityID);

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