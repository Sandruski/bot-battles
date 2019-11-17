#include "Game.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "FSM.h"
#include "ResourceManager.h"
#include "SystemManager.h"

#include "LinkingContext.h"

#include "InputSystem.h"
#include "NavigationSystem.h"
#include "ServerSystem.h"
#include "SpawnerSystem.h"

#include "SingletonServerComponent.h"

#include "InputComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game(const GameConfiguration& configuration)
    : m_configuration(configuration)
    , m_timer()
    , m_lastFrameMs(0)
    , m_desiredFramerate(60.0)
    , m_fps(0)
    , m_dt(0)
    , m_isRunning(false)
{
    m_entityManager = std::make_shared<EntityManager>();
    m_componentManager = std::make_shared<ComponentManager>();
    m_systemManager = std::make_shared<SystemManager>();
    m_resourceManager = std::make_shared<ResourceManager>();
    m_fsm = std::make_shared<FSM>();

    m_linkingContext = std::make_unique<LinkingContext>();

    m_singletonServerComponent = std::make_shared<SingletonServerComponent>();
}

//----------------------------------------------------------------------------------------------------
Game::~Game()
{
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
    // Systems
    bool ret = m_systemManager->RegisterSystem<ServerSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<SpawnerSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<InputSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<NavigationSystem>();
    if (!ret) {
        return false;
    }

    // Components
    ret = m_componentManager->RegisterComponent<TransformComponent>();
    if (!ret) {
        return false;
    }
    ret = m_componentManager->RegisterComponent<InputComponent>();
    if (!ret) {
        return false;
    }

    // Observers (add own class last)
    ret = m_entityManager->AddObserver(m_componentManager);
    if (!ret) {
        return false;
    }

    ret = m_entityManager->AddObserver(m_systemManager);
    if (!ret) {
        return false;
    }

    ret = m_entityManager->AddObserver(m_entityManager);
    if (!ret) {
        return false;
    }

    ret = m_componentManager->AddObserver(m_entityManager);
    if (!ret) {
        return false;
    }

    ret = m_componentManager->AddObserver(m_componentManager);
    if (!ret) {
        return false;
    }

    std::shared_ptr<ServerSystem> server = m_systemManager->GetSystem<ServerSystem>();
    ret = m_entityManager->AddObserver(server);
    if (!ret) {
        return false;
    }

    ret = server->AddObserver(server);
    if (!ret) {
        return false;
    }

    std::shared_ptr<SpawnerSystem> spawner = m_systemManager->GetSystem<SpawnerSystem>();
    ret = server->AddObserver(spawner);
    if (!ret) {
        return false;
    }

    ret = m_systemManager->StartUp();
    if (!ret) {
        return false;
    }

    if (m_configuration.StatesSetup != nullptr) {
        m_configuration.StatesSetup();
    }

    m_isRunning = true;

    return true;
}

//----------------------------------------------------------------------------------------------------
// game loop
bool Game::Update()
{
    m_timer.Start();

    // PreUpdate
    bool ret = m_entityManager->PreUpdate();
    if (!ret) {
        return false;
    }

    ret = m_componentManager->PreUpdate();
    if (!ret) {
        return false;
    }

    ret = m_systemManager->PreUpdate();
    if (!ret) {
        return false;
    }

    ret = m_fsm->PreUpdate();
    if (!ret) {
        return false;
    }

    // Update
    ret = m_systemManager->Update();
    if (!ret) {
        return false;
    }

    ret = m_fsm->Update();
    if (!ret) {
        return false;
    }

    // PostUpdate
    ret = m_systemManager->PostUpdate();
    if (!ret) {
        return false;
    }

    ret = m_fsm->PostUpdate();
    if (!ret) {
        return false;
    }

    // Render
    ret = m_systemManager->Render();
    if (!ret) {
        return false;
    }

    ret = m_fsm->Render();
    if (!ret) {
        return false;
    }

    m_lastFrameMs = m_timer.ReadMs();
    F64 desiredLastFrameMs = 1000.0 / m_desiredFramerate;
    if (m_lastFrameMs < desiredLastFrameMs) {
        SDL_Delay(static_cast<U32>(desiredLastFrameMs - m_lastFrameMs));
        m_lastFrameMs = m_timer.ReadMs();
    }

    m_fps = 1000.0 / m_lastFrameMs;
    m_dt = 1.0 / m_fps;

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::End()
{
    bool ret = m_systemManager->ShutDown();
    if (!ret) {
        return false;
    }

    ret = m_resourceManager->ShutDown();
    if (!ret) {
        return false;
    }

    ret = m_fsm->ShutDown();
    if (!ret) {
        return false;
    }

    return ret;
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
