#include "Game.h"

#include "BotComponent.h"
#include "BotSpawnerComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "Config.h"
#include "DisplayPanel.h"
#include "EntityManager.h"
#include "EventComponent.h"
#include "EventSystem.h"
#include "FSM.h"
#include "FileSystem.h"
#include "GamePanel.h"
#include "GameplayComponent.h"
#include "GraphicsPanel.h"
#include "GuiComponent.h"
#include "GuiSystem.h"
#include "HealthComponent.h"
#include "HealthSpawnerComponent.h"
#include "LabelComponent.h"
#include "LabelSystem.h"
#include "LinkingContext.h"
#include "MainMenuComponent.h"
#include "MapComponent.h"
#include "MapImporter.h"
#include "MapSystem.h"
#include "MeshImporter.h"
#include "NetworkingPanel.h"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "PlayerComponent.h"
#include "RendererComponent.h"
#include "RendererSystem.h"
#include "ResourceManager.h"
#include "RigidbodyComponent.h"
#include "ScoreboardComponent.h"
#include "ShaderImporter.h"
#include "SightComponent.h"
#include "SpriteComponent.h"
#include "StatesPanel.h"
#include "SystemManager.h"
#include "TextureImporter.h"
#include "TransformComponent.h"
#include "WallComponent.h"
#include "WeaponComponent.h"
#include "WeaponSpawnerComponent.h"
#include "WindowComponent.h"
#include "WindowSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game()
    : m_isRunning(false)
    , m_config()
    , m_entityManager()
    , m_componentManager()
    , m_systemManager()
    , m_fsm()
    , m_linkingContext()
    , m_resourceManager()
    , m_fileSystem()
    , m_textureImporter()
    , m_meshImporter()
    , m_shaderImporter()
    , m_mapImporter()
    , m_windowComponent()
    , m_rendererComponent()
    , m_EventComponent()
    , m_guiComponent()
    , m_physicsComponent()
    , m_mapComponent()
    , m_mainMenuComponent()
    , m_gameplayComponent()
    , m_scoreboardComponent()
{
    m_entityManager = std::make_shared<EntityManager>();
    m_componentManager = std::make_shared<ComponentManager>();
    m_systemManager = std::make_shared<SystemManager>();
    m_fsm = std::make_shared<FSM>();
    m_linkingContext = std::make_shared<LinkingContext>();
    m_resourceManager = std::make_shared<ResourceManager>();
    m_fileSystem = std::make_shared<FileSystem>();
    m_textureImporter = std::make_shared<TextureImporter>();
    m_meshImporter = std::make_shared<MeshImporter>();
    m_shaderImporter = std::make_shared<ShaderImporter>();
    m_mapImporter = std::make_shared<MapImporter>();
    m_windowComponent = std::make_shared<WindowComponent>();
    m_rendererComponent = std::make_shared<RendererComponent>();
    m_EventComponent = std::make_shared<EventComponent>();
    m_guiComponent = std::make_shared<GuiComponent>();
    m_physicsComponent = std::make_shared<PhysicsComponent>();
    m_mapComponent = std::make_shared<MapComponent>();
    m_mainMenuComponent = std::make_shared<MainMenuComponent>();
    m_gameplayComponent = std::make_shared<GameplayComponent>();
    m_scoreboardComponent = std::make_shared<ScoreboardComponent>();
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
    bool ret = false;

    // Systems
    ret = m_systemManager->RegisterSystem<WindowSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<RendererSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<LabelSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<GuiSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<EventSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<PhysicsSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<MapSystem>();
    if (!ret) {
        return ret;
    }

    // Components
    ret = m_componentManager->RegisterComponent<TransformComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<ColliderComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<RigidbodyComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<LabelComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<SpriteComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<PlayerComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<WeaponComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<HealthComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<SightComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<BotComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<WallComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<BotSpawnerComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<WeaponSpawnerComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<HealthSpawnerComponent>();
    if (!ret) {
        return ret;
    }

    // Panels
    ret = m_guiComponent->RegisterDebugOptionsPanel<GamePanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent->RegisterDebugOptionsPanel<GraphicsPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent->RegisterDebugOptionsPanel<StatesPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent->RegisterDebugOptionsPanel<NetworkingPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent->SetCurrentDebugOptionsPanel<GamePanel>();
    if (!ret) {
        return ret;
    }

    ret = m_guiComponent->RegisterSettingsPanel<DisplayPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent->SetCurrentSettingsPanel<DisplayPanel>();
    if (!ret) {
        return ret;
    }

    ret = m_entityManager->AddObserver(std::weak_ptr<Observer>(m_componentManager));
    if (!ret) {
        return ret;
    }
    ret = m_entityManager->AddObserver(std::weak_ptr<Observer>(m_systemManager));
    if (!ret) {
        return ret;
    }
    ret = m_entityManager->AddObserver(std::weak_ptr<Observer>(m_entityManager));
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->AddObserver(std::weak_ptr<Observer>(m_entityManager));
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->AddObserver(std::weak_ptr<Observer>(m_componentManager));
    if (!ret) {
        return ret;
    }
    std::weak_ptr<PhysicsSystem> physicsSystem = m_systemManager->GetSystem<PhysicsSystem>();
    ret = m_componentManager->AddObserver(physicsSystem);
    if (!ret) {
        return ret;
    }
    ret = m_fsm->AddObserver(std::weak_ptr<Observer>(m_fsm));
    if (!ret) {
        return ret;
    }
    std::weak_ptr<RendererSystem> rendererSystem = m_systemManager->GetSystem<RendererSystem>();
    ret = m_systemManager->AddObserver(rendererSystem);
    if (!ret) {
        return ret;
    }
    ret = m_windowComponent->AddObserver(rendererSystem);
    if (!ret) {
        return ret;
    }

    m_config->LoadFromJson();

    ret = m_systemManager->StartUp();
    if (!ret) {
        return ret;
    }
    ret = m_fsm->StartUp();
    if (!ret) {
        return ret;
    }

    m_isRunning = true;

    return ret;
}

//----------------------------------------------------------------------------------------------------
void Game::InitFrame()
{
    MyTime::GetInstance().StartUpdate();
}

//----------------------------------------------------------------------------------------------------
bool Game::DoFrame()
{
    bool ret = PreUpdate();
    if (!ret) {
        return ret;
    }

    ret = Update();
    if (!ret) {
        return ret;
    }

    ret = PostUpdate();
    if (!ret) {
        return ret;
    }

    ret = PreRender();
    if (!ret) {
        return ret;
    }

    ret = Render();
    if (!ret) {
        return ret;
    }

    ret = DebugRender();
    if (!ret) {
        return ret;
    }

    ret = RenderGui();
    if (!ret) {
        return ret;
    }

    ret = PostRender();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
void Game::EndFrame()
{
    MyTime::GetInstance().FinishUpdate();
}

//----------------------------------------------------------------------------------------------------
bool Game::End()
{
    bool ret = false;

    ret = m_fsm->ShutDown();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->ShutDown();
    if (!ret) {
        return ret;
    }
    ret = m_resourceManager->ShutDown();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PreUpdate()
{
    bool ret = false;

    ret = m_physicsComponent->PreUpdate();
    if (!ret) {
        return ret;
    }
    ret = m_windowComponent->PreUpdate();
    if (!ret) {
        return ret;
    }
    ret = m_entityManager->PreUpdate();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->PreUpdate();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->PreUpdate();
    if (!ret) {
        return ret;
    }
    ret = m_linkingContext->PreUpdate();
    if (!ret) {
        return ret;
    }
    ret = m_fsm->PreUpdate();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::Update()
{
    bool ret = false;

    ret = m_systemManager->Update();
    if (!ret) {
        return ret;
    }
    ret = m_fsm->Update();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PostUpdate()
{
    bool ret = false;

    ret = m_systemManager->PostUpdate();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PreRender()
{
    return m_systemManager->PreRender();
}

//----------------------------------------------------------------------------------------------------
bool Game::Render()
{
    return m_systemManager->Render();
}

//----------------------------------------------------------------------------------------------------
bool Game::DebugRender()
{
    bool ret = true;

    if (m_rendererComponent->m_isDebugDraw) {
        ret = m_systemManager->DebugRender();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::RenderGui()
{
    bool ret = false;

    ret = m_fsm->RenderGui();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RenderGui();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PostRender()
{
    return m_systemManager->PostRender();
}
}
