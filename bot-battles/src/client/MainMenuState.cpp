#include "MainMenuState.h"

#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "MeshComponent.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

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
    Entity background = g_gameClient->GetEntityManager().AddEntity();

    std::weak_ptr<TransformComponent> transform = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    transform.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x / 2), static_cast<F32>(windowComponent.m_resolution.y / 2), 0.0f };

    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("Sample.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    std::weak_ptr<MeshComponent> meshComponent = g_gameClient->GetComponentManager().AddComponent<MeshComponent>(background);
    meshComponent.lock()->Init();

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
    g_gameClient->GetEntityManager().ClearEntities();
    ILOG("Exit MainMenuState");

    return true;
}
}
