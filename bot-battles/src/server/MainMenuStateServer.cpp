#include "MainMenuStateServer.h"

#include "ComponentManager.h"
#include "ConnectStateServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "GuiComponent.h"
#include "LabelComponent.h"
#include "MainMenuComponent.h"
#include "ResourceManager.h"
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

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    ret = mainMenuComponent.lock()->m_fsm.RegisterState<SetupStateServer>();
    if (!ret) {
        return ret;
    }
    ret = mainMenuComponent.lock()->m_fsm.RegisterState<ConnectStateServer>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Enter() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_gameServer->GetWindowComponent();

    // Scene
    Entity background = g_gameServer->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position += static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
    transformComponent.lock()->m_layerType = LayerType::BACKGROUND;
    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("mainMenuServer.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    mainMenuComponent.lock()->m_author = g_gameServer->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> authorTransformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(mainMenuComponent.lock()->m_author);
    authorTransformComponent.lock()->m_position.x = 0.0f;
    authorTransformComponent.lock()->m_position.y = static_cast<F32>(windowComponent.lock()->m_baseResolution.y);
    std::weak_ptr<LabelComponent> authorLabelComponent = g_gameServer->GetComponentManager().AddComponent<LabelComponent>(mainMenuComponent.lock()->m_author);
    authorLabelComponent.lock()->m_text = "(c) 2020 Sandra Alvarez";
    authorLabelComponent.lock()->m_color = Black;
    ImVec2 authorTextSize = ImGui::CalcTextSize(authorLabelComponent.lock()->m_text.c_str());
    authorLabelComponent.lock()->m_offset = glm::vec2(5.0f, -5.0f);
    authorLabelComponent.lock()->m_extraOffset = glm::vec2(authorTextSize.x / 2.0f, -authorTextSize.y / 2.0f);

    mainMenuComponent.lock()->m_version = g_gameServer->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> versionTransformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(mainMenuComponent.lock()->m_version);
    versionTransformComponent.lock()->m_position.x = static_cast<F32>(windowComponent.lock()->m_baseResolution.x);
    versionTransformComponent.lock()->m_position.y = static_cast<F32>(windowComponent.lock()->m_baseResolution.y);
    std::weak_ptr<LabelComponent> versionLabelComponent = g_gameServer->GetComponentManager().AddComponent<LabelComponent>(mainMenuComponent.lock()->m_version);
    versionLabelComponent.lock()->m_text = "v1.0";
    versionLabelComponent.lock()->m_color = Black;
    ImVec2 versionTextSize = ImGui::CalcTextSize(versionLabelComponent.lock()->m_text.c_str());
    versionLabelComponent.lock()->m_offset = glm::vec2(-5.0f, -5.0f);
    versionLabelComponent.lock()->m_extraOffset = glm::vec2(-versionTextSize.x / 2.0f, -versionTextSize.y / 2.0f);

    std::weak_ptr<GuiComponent> guiComponent = g_gameServer->GetGuiComponent();
    guiComponent.lock()->m_isSettings = false;

    return mainMenuComponent.lock()->m_fsm.ChangeState("Setup");
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Update() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    return mainMenuComponent.lock()->m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::RenderGui() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    return mainMenuComponent.lock()->m_fsm.RenderGui();
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Exit() const
{
    // Scene
    g_gameServer->GetEntityManager().ClearEntities();

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    mainMenuComponent.lock()->m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateServer::OnNotify(const Event& event)
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.lock()->m_fsm.OnNotify(event);
}
}
