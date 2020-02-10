#include "MainMenuState.h"

#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MainMenuState::MainMenuState()
    : m_background(INVALID_ENTITY)
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
    m_background = g_gameClient->GetEntityManager().AddEntity();

    std::weak_ptr<TransformComponent> transform = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(m_background);
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    transform.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x / 2), static_cast<F32>(windowComponent.m_resolution.y / 2), 0.0f };

    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(m_background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

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
    g_gameClient->GetEntityManager().RemoveEntity(m_background);

    return true;
}
}
