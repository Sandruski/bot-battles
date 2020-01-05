#include "GameplayState.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayState::GameplayState()
{
}

//----------------------------------------------------------------------------------------------------
const char* GameplayState::GetName()
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Enter()
{
    Entity character = g_game->GetEntityManager().AddEntity();

    std::weak_ptr<TransformComponent> transform = g_game->GetComponentManager().AddComponent<TransformComponent>(character);

    std::weak_ptr<SpriteResource> spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("character.png", "../../data/baker_house/", true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().AddComponent<SpriteComponent>(character);
    spriteComponent.lock()->m_sprite = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::PreUpdate()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Update()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::PostUpdate()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Exit()
{
    return true;
}
}
