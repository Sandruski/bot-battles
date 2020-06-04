#include "ScoreboardStateServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ConfigServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "GuiComponent.h"
#include "ResourceManager.h"
#include "RestartStateServer.h"
#include "ResultsStateServer.h"
#include "ScoreboardComponent.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string ScoreboardStateServer::GetName() const
{
    return "Scoreboard";
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Create() const
{
    bool ret = false;

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    ret = scoreboardComponent.lock()->m_fsm.RegisterState<ResultsStateServer>();
    if (!ret) {
        return ret;
    }
    ret = scoreboardComponent.lock()->m_fsm.RegisterState<RestartStateServer>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Enter() const
{
    // Scene
    Entity background = g_gameServer->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    std::weak_ptr<WindowComponent> windowComponent = g_gameServer->GetWindowComponent();
    transformComponent.lock()->m_position += static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
    transformComponent.lock()->m_layerType = LayerType::BACKGROUND;
    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("scoreboardBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    std::weak_ptr<GuiComponent> guiComponent = g_gameServer->GetGuiComponent();
    guiComponent.lock()->m_isSettings = false;

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    ++scoreboardComponent.lock()->m_gameCount;
    return scoreboardComponent.lock()->m_fsm.ChangeState("Results");
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Update() const
{
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    return scoreboardComponent.lock()->m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::RenderGui() const
{
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    return scoreboardComponent.lock()->m_fsm.RenderGui();
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Exit() const
{
    // Scene
    g_gameServer->GetEntityManager().ClearEntities();

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    scoreboardComponent.lock()->m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateServer::OnNotify(const Event& event)
{
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.lock()->m_fsm.OnNotify(event);
}
}
