#include "MainMenuState.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MainMenuState::MainMenuState()
{
}

//----------------------------------------------------------------------------------------------------
const char* MainMenuState::GetName()
{
    return "MainMenu";
}

//----------------------------------------------------------------------------------------------------
bool MainMenuState::Enter()
{
    Entity character = g_game->GetEntityManager().AddEntity();

    std::weak_ptr<TransformComponent> transform = g_game->GetComponentManager().AddComponent<TransformComponent>(character);

    std::weak_ptr<SpriteResource> spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("character.png", "../../data/baker_house/", true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().AddComponent<SpriteComponent>(character);
    spriteComponent.lock()->m_sprite = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuState::PreUpdate()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuState::Update()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuState::PostUpdate()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuState::Exit()
{
    return true;
}
}
