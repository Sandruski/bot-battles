#include "GameplayStateServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "MapImporter.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayStateServer::GameplayStateServer()
{
}

//----------------------------------------------------------------------------------------------------
const char* GameplayStateServer::GetName()
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Enter()
{
    /*
    Entity background = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(background);

    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x / 2), static_cast<F32>(windowComponent.m_resolution.y / 2), 0.0f };

    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("gameplayBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    std::weak_ptr<MeshComponent> meshComponent = g_gameServer->GetComponentManager().AddComponent<MeshComponent>(background);
    meshComponent.lock()->Init();
    */
    std::string path;
    path.append(MAPS_DIR);
    path.append("map.json");
    g_gameServer->GetMapImporter().Load(path);
    //WindowComponent& windowComponent = g_game->GetWindowComponent();
    //transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x / 2), static_cast<F32>(windowComponent.m_resolution.y / 2), 0.0f };

    //realPosition -= glm::vec2(static_cast<F32>(mapComponent.lock()->m_size.x * mapComponent.lock()->m_tileSize.x) / 2.0f,
    //static_cast<F32>(mapComponent.lock()->m_size.y * mapComponent.lock()->m_tileSize.y) / 2);
    //realPosition += glm::vec2(mapComponent.lock()->m_tileSize.x / 2, mapComponent.lock()->m_tileSize.y / 2);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::PreUpdate()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Update()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::PostUpdate()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Exit()
{
    g_game->GetEntityManager().ClearEntities();
    g_game->GetLinkingContext().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        OnPlayerAdded();
        break;
    }

    case EventType::PLAYER_REMOVED: {
        OnPlayerRemoved();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnPlayerAdded() const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    U32 playerCount = serverComponent.GetPlayerCount();
    if (playerCount < MAX_PLAYER_IDS) {
        GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
        gameplayComponent.m_phaseType = PhaseType::PLAY;
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnPlayerRemoved() const
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    if (gameplayComponent.m_phaseType == PhaseType::PLAY) {
        gameplayComponent.m_phaseType = PhaseType::RESTART;
    }
}
}
