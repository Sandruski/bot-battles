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
    Entity background = g_game->GetEntityManager().AddEntity();

    std::weak_ptr<TransformComponent> transform = g_game->GetComponentManager().AddComponent<TransformComponent>(background);

    std::weak_ptr<SpriteResource> spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("gameplayBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().AddComponent<SpriteComponent>(background);
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
