#include "MainMenuStateServer.h"

#include "ComponentManager.h"
#include "ConnectStateServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "MainMenuComponent.h"
#include "SetupStateServer.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string MainMenuStateServer::GetName() const
{
    return "Main Menu";
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Create() const
{
    bool ret = false;

    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    ret = mainMenuComponent.m_fsm.RegisterState<SetupStateServer>();
    if (!ret) {
        return ret;
    }
    ret = mainMenuComponent.m_fsm.RegisterState<ConnectStateServer>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Enter() const
{
    // Scene
    Entity background = g_gameServer->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_layerType = LayerType::BACKGROUND;
    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    return mainMenuComponent.m_fsm.ChangeState("Setup");
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Update() const
{
    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    return mainMenuComponent.m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::RenderGui() const
{
    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    return mainMenuComponent.m_fsm.RenderGui();
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Exit() const
{
    // Scene
    g_gameServer->GetEntityManager().ClearEntities();

    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    mainMenuComponent.m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateServer::OnNotify(const Event& event)
{
    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.m_fsm.OnNotify(event);
}
}
