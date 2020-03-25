#include "ScoreboardStateServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "ScoreboardComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* ScoreboardStateServer::GetName() const
{
    return "Scoreboard";
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Enter() const
{
    ILOG("Entering %s...", GetName());

    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::RESULTS;

    Entity background = g_gameServer->GetEntityManager().AddEntity();

    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(LayerType::BACKGROUND) };

    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("scoreboardBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName());

    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::NONE;

    g_gameServer->GetEntityManager().ClearEntities();

    return true;
}
}
