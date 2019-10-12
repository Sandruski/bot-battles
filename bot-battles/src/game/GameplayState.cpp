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
    : m_resourceTexture(nullptr)
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

    Entity entity = g_game->GetEntityManager().AddEntity();
    Entity entity2 = g_game->GetEntityManager().AddEntity();
    auto transform = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
    auto transform2 = g_game->GetComponentManager().AddComponent<TransformComponent>(entity2);
    auto sprite = g_game->GetComponentManager().AddComponent<SpriteComponent>(entity);
    auto sprite2 = g_game->GetComponentManager().AddComponent<SpriteComponent>(entity2);

    bool removed = g_game->GetComponentManager().RemoveComponent<TransformComponent>(entity);

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
