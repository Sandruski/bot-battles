#include "Game.h"

#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "EventSystem.h"
#include "FSM.h"
#include "GameplayState.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#ifdef _DRAW
#include "HUDSystem.h"
#include "RendererSystem.h"
#include "WindowSystem.h"

#include "SpriteComponent.h"
#include "TextComponent.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game()
    : m_config(nullptr)
    , m_entityManager()
    , m_componentManager()
    , m_systemManager()
    , m_linkingContext()
    , m_fsm()
#ifdef _DRAW
    , m_fontImporter()
    , m_textureImporter()
    , m_resourceManager()
    , m_windowComponent()
    , m_rendererComponent()
#endif
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
#endif
    ret = m_systemManager->RegisterSystem<EventSystem>();
    if (!ret) {
        return ret;
    }

    // Components
#ifdef _DRAW
    ret = m_componentManager->RegisterComponent<SpriteComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<TextComponent>();
    if (!ret) {
        return ret;
    }
#endif
    ret = m_componentManager->RegisterComponent<TransformComponent>();
    if (!ret) {
        return ret;
    }

    // States
    ret = m_fsm->RegisterState<GameplayState>();
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

#ifdef _DRAW
    ret = m_textureImporter.StartUp();
    if (!ret) {
        return ret;
    }
    ret = m_fontImporter.StartUp();
    if (!ret) {
        return ret;
    }
#endif

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
    Time::GetInstance().StartUpdate();
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
    Time::GetInstance().FinishUpdate();
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
    ret = m_fontImporter.ShutDown();
    if (!ret) {
        return ret;
    }
    ret = m_textureImporter.ShutDown();
    if (!ret) {
        return ret;
    }
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
    ret = m_fsm->PreUpdate();
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
    ret = m_fsm->PostUpdate();
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
    bool ret = false;

    ret = m_systemManager->Render();
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
