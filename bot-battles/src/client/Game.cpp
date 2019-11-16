#include "Game.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "FSM.h"
#include "ResourceManager.h"
#include "SystemManager.h"
#include "TextureImporter.h"

#include "ClientReplicationManager.h"
#include "LinkingContext.h"

#include "ClientSystem.h"
#include "EventSystem.h"
#include "InputSystem.h"
#include "RendererSystem.h"
#include "WindowSystem.h"

#include "SingletonClientComponent.h"
#include "SingletonInputComponent.h"
#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"

#include "InputComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game(const GameConfiguration& configuration)
    : m_configuration(configuration)
    , m_isRunning(false)
{
    m_entityManager = std::make_shared<EntityManager>();
    m_componentManager = std::make_shared<ComponentManager>();
    m_systemManager = std::make_shared<SystemManager>();
    m_resourceManager = std::make_shared<ResourceManager>();
    m_fsm = std::make_shared<FSM>();
    m_textureImporter = std::make_shared<TextureImporter>();

    m_linkingContext = std::make_unique<LinkingContext>();
    m_clientReplicationManager = std::make_unique<ClientReplicationManager>();

    m_singletonInputComponent = std::make_shared<SingletonInputComponent>();
    m_singletonRendererComponent = std::make_shared<SingletonRendererComponent>();
    m_singletonWindowComponent = std::make_shared<SingletonWindowComponent>();
    m_singletonClientComponent = std::make_shared<SingletonClientComponent>();
}

//----------------------------------------------------------------------------------------------------
Game::~Game()
{
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
    // Systems
    bool ret = m_systemManager->RegisterSystem<WindowSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<RendererSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<EventSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<InputSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<ClientSystem>();
    if (!ret) {
        return false;
    }

    // Components
    ret = m_componentManager->RegisterComponent<TransformComponent>();
    if (!ret) {
        return false;
    }
    ret = m_componentManager->RegisterComponent<SpriteComponent>();
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

    ret = m_systemManager->StartUp();
    if (!ret) {
        return false;
    }

    ret = m_textureImporter->StartUp();
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
    Time::GetInstance().StartUpdate();

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

    Time::GetInstance().FinishUpdate();

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
