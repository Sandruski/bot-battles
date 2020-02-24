#include "GameplayStateServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
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
    Entity background = g_gameServer->GetEntityManager().AddEntity();

    std::weak_ptr<TransformComponent> transform = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    transform.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x / 2), static_cast<F32>(windowComponent.m_resolution.y / 2), 0.0f };

    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("gameplayBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

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
    ILOG("Exit GameplayState");

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
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnPlayerRemoved() const
{
}
}
