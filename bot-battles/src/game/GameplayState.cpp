#include "GameplayState.h"

#include "Game.h"
#include "ResourceManager.h"
#include "ResourceTexture.h"

#include "ComponentManager.h"
#include "EntityManager.h"

#include "SpriteComponent.h"
#include "TransformComponent.h"

#include "WorkingDirDefs.h"

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
	auto texture = g_game->GetResourceManager().AddResource<ResourceTexture>("baker_house.png", TEXTURES_DIR);
	sprite->m_texture = texture;

	Entity character = g_game->GetEntityManager().AddEntity();
	auto sprite2 = g_game->GetComponentManager().AddComponent<SpriteComponent>(character);
	auto texture2 = g_game->GetResourceManager().AddResource<ResourceTexture>("character.png", TEXTURES_DIR);
	sprite2->m_texture = texture2;
	auto transform = g_game->GetComponentManager().AddComponent<TransformComponent>(character);
	transform->m_position = Vec2(10.0f, 40.0f);

	g_game->GetEntityManager().RemoveEntity(character);

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
