#include "Game.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "EventSystem.h"
#include "InputComponent.h"
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
Game::Game(const char* configPath)
    : m_config(configPath)
    , m_configuration()
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
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
    bool ret = false;

    m_config.LoadFromJson();

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
    ret = m_componentManager->RegisterComponent<InputComponent>();
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
    ret = m_systemManager->StartUp();
    if (!ret) {
        return ret;
    }

    if (m_configuration.StatesSetup != nullptr) {
        m_configuration.StatesSetup();
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
        return false;
    }

    ret = Update();
    if (!ret) {
        return false;
    }

    ret = PostUpdate();
    if (!ret) {
        return false;
    }

    ret = PreRender();
    if (!ret) {
        return false;
    }

    ret = Render();
    if (!ret) {
        return false;
    }

    ret = PostRender();
    if (!ret) {
        return false;
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

    ret = m_systemManager->ShutDown();
    if (!ret) {
        return false;
    }
#ifdef _DRAW
    ret = m_fontImporter.ShutDown();
    if (!ret) {
        return false;
    }
    ret = m_textureImporter.ShutDown();
    if (!ret) {
        return false;
    }
    ret = m_resourceManager.ShutDown();
    if (!ret) {
        return false;
    }
#endif
    ret = m_fsm.ShutDown();
    if (!ret) {
        return false;
    }

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
    ret = m_fsm.PreUpdate();
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
    ret = m_fsm.Update();
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
    ret = m_fsm.PostUpdate();
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
    ret = m_fsm.Render();
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

//----------------------------------------------------------------------------------------------------
GameConfiguration::GameConfiguration()
    : m_name(nullptr)
    , StatesSetup(nullptr)
{
}

//----------------------------------------------------------------------------------------------------
GameConfiguration::GameConfiguration(const char* name, std::function<void()> StatesSetup)
    : m_name(name)
    , StatesSetup(StatesSetup)
{
}

//----------------------------------------------------------------------------------------------------
GameConfiguration::GameConfiguration(const GameConfiguration& configuration)
    : m_name(configuration.m_name)
    , StatesSetup(configuration.StatesSetup)
{
}
}
