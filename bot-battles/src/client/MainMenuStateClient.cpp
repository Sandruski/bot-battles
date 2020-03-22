#include "MainMenuStateClient.h"

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
MainMenuStateClient::MainMenuStateClient()
{
}

//----------------------------------------------------------------------------------------------------
const char* MainMenuStateClient::GetName()
{
    return "MainMenu";
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Enter()
{
    Entity background = g_gameClient->GetEntityManager().AddEntity();

    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, 0.0f };

    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::RenderGui()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Exit()
{
    g_gameClient->GetEntityManager().ClearEntities();
    ILOG("Exit MainMenuState");

    return true;
}
}
