#include "Game.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "EventSystem.h"
#include "FSM.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "SpawnComponent.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#ifdef _DRAW
#include "DisplayPanel.h"
#include "GamePanel.h"
#include "GraphicsPanel.h"
#include "GuiSystem.h"
#include "HUDSystem.h"
#include "LabelComponent.h"
#include "NetworkingPanel.h"
#include "RendererSystem.h"
#include "ShaderResource.h"
#include "SpriteComponent.h"
#include "StatesPanel.h"
#include "WindowSystem.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game()
    : m_config(nullptr)
    , m_entityManager()
    , m_componentManager()
    , m_systemManager()
    , m_fsm()
    , m_linkingContext()
#ifdef _DRAW
    , m_shaderImporter()
    , m_textureImporter()
    , m_windowComponent()
    , m_rendererComponent()
#endif
    , m_resourceManager()
    , m_mapImporter()
    , m_eventComponent()
    , m_mainMenuComponent()
    , m_gameplayComponent()
    , m_scoreboardComponent()
    , m_isRunning(false)
{
    m_entityManager = std::make_shared<EntityManager>();
    m_componentManager = std::make_shared<ComponentManager>();
    m_systemManager = std::make_shared<SystemManager>();
    m_fsm = std::make_shared<FSM>();
    m_linkingContext = std::make_shared<LinkingContext>();
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
    bool ret = false;

    // Systems
#ifdef _DRAW
    ret = m_systemManager->RegisterSystem<WindowSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<RendererSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<HUDSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<GuiSystem>();
    if (!ret) {
        return ret;
    }
#endif
    ret = m_systemManager->RegisterSystem<EventSystem>();
    if (!ret) {
        return ret;
    }

    // Components
#ifdef _DRAW
    ret = m_componentManager->RegisterComponent<LabelComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<SpriteComponent>();
    if (!ret) {
        return ret;
    }
#endif
    ret = m_componentManager->RegisterComponent<TransformComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<ColliderComponent>();
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
    ret = m_componentManager->RegisterComponent<SpawnComponent>();
    if (!ret) {
        return ret;
    }

    // Panels
#ifdef _DRAW
    ret = m_guiComponent.RegisterDebugOptionsPanel<GamePanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent.RegisterDebugOptionsPanel<GraphicsPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent.RegisterDebugOptionsPanel<StatesPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent.RegisterDebugOptionsPanel<NetworkingPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent.SetCurrentDebugOptionsPanel<GamePanel>();
    if (!ret) {
        return ret;
    }

    ret = m_guiComponent.RegisterSettingsPanel<DisplayPanel>();
    if (!ret) {
        return ret;
    }
    ret = m_guiComponent.SetCurrentSettingsPanel<DisplayPanel>();
    if (!ret) {
        return ret;
    }
#endif

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
    InitFrame();

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

    EndFrame();

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
#ifdef _DRAW
    ret = m_resourceManager.ShutDown();
    if (!ret) {
        return ret;
    }
#endif

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PreUpdate()
{
    bool ret = false;

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

    if (m_rendererComponent.m_isDebugDraw) {
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
