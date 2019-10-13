#include "GameplayState.h"

#include "Game.h"
#include "ResourceManager.h"
#include "ResourceTexture.h"

#include "ComponentManager.h"
#include "EntityManager.h"

#include "SpriteComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayState::GameplayState()
{
}

//----------------------------------------------------------------------------------------------------
GameplayState::~GameplayState()
{
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Create()
{
    Entity background = g_game->GetEntityManager().AddEntity();
    auto sprite = g_game->GetComponentManager().AddComponent<SpriteComponent>(background);
	auto texture = g_game->GetResourceManager().AddResource<ResourceTexture>("baker_house.png", "../../data/textures/");
	sprite->m_texture = texture;

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
