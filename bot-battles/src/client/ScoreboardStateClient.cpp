#include "ScoreboardStateClient.h"

#include "ComponentManager.h"
#include "ConfigClient.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "RestartStateClient.h"
#include "ResultsStateClient.h"
#include "ScoreboardComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string ScoreboardStateClient::GetName() const
{
    return "Scoreboard";
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Create() const
{
    bool ret = false;

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    ret = scoreboardComponent.m_fsm.RegisterState<ResultsStateClient>();
    if (!ret) {
        return ret;
    }
    ret = scoreboardComponent.m_fsm.RegisterState<RestartStateClient>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Enter() const
{
    // Scene
    Entity background = g_gameClient->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_layerType = LayerType::BACKGROUND;
    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("scoreboardBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.m_mainMenuTimer.Start();
    return scoreboardComponent.m_fsm.ChangeState("Results");
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Update() const
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    return scoreboardComponent.m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::RenderGui() const
{
    bool ret = false;

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    glm::uvec2 resolution = windowComponent.GetResolution();
    ImVec2 position = ImVec2(static_cast<F32>(resolution.x) / 2.0f, static_cast<F32>(resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(resolution.y) / 2.0f, static_cast<F32>(resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    if (ImGui::Begin(GetName().c_str(), nullptr, windowFlags)) {
        ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
        ret = scoreboardComponent.m_fsm.RenderGui();

        ImGui::End();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Exit() const
{
    // Scene
    g_gameClient->GetEntityManager().ClearEntities();

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    scoreboardComponent.m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateClient::OnNotify(const Event& event)
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.m_fsm.OnNotify(event);
}
}
