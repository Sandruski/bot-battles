#include "Game.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "FSM.h"
#include "LinkingContext.h"
#include "SystemManager.h"
#ifdef _DRAW
#include "ResourceManager.h"
#include "TextureImporter.h"

#include "RendererSystem.h"
#include "WindowSystem.h"
#include "HUDSystem.h"
#endif
#include "EventSystem.h"

#ifdef _DRAW
#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"
#include "SpriteComponent.h"
#endif
#include "InputComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game(const GameConfiguration& configuration)
    : m_configuration(configuration)
    , m_entityManager()
    , m_componentManager()
    , m_systemManager()
    , m_linkingContext()
    , m_resourceManager()
    , m_textureImporter()
#ifdef _DRAW
    , m_singletonWindowComponent()
    , m_singletonRendererComponent()
#endif
    , m_isRunning(false)
{
    m_entityManager = std::make_shared<EntityManager>();
    m_componentManager = std::make_shared<ComponentManager>();
    m_systemManager = std::make_shared<SystemManager>();
    m_linkingContext = std::make_unique<LinkingContext>();
    m_fsm = std::make_shared<FSM>(); // TODO: remove this!
#ifdef _DRAW
    m_resourceManager = std::make_shared<ResourceManager>();
    m_textureImporter = std::make_shared<TextureImporter>();

    m_singletonRendererComponent = std::make_shared<SingletonRendererComponent>();
    m_singletonWindowComponent = std::make_shared<SingletonWindowComponent>();
#endif
}

//----------------------------------------------------------------------------------------------------
Game::~Game()
{
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
    // Systems
    bool ret = false;

#ifdef _DRAW
    ret = m_systemManager->RegisterSystem<WindowSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<RendererSystem>();
    if (!ret) {
        return false;
    }
	ret = m_systemManager->RegisterSystem<HUDSystem>();
	if (!ret) {
		return false;
	}
#endif
	ret = m_systemManager->RegisterSystem<EventSystem>();
	if (!ret) {
		return false;
	}

    // Components
#ifdef _DRAW
    ret = m_componentManager->RegisterComponent<SpriteComponent>();
    if (!ret) {
        return false;
    }
#endif
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

    ret = m_systemManager->StartUp();
    if (!ret) {
        return false;
    }

#ifdef _DRAW
    ret = m_textureImporter->StartUp();
    if (!ret) {
        return false;
    }
#endif

    if (m_configuration.StatesSetup != nullptr) {
        m_configuration.StatesSetup();
    }

    m_isRunning = true;

    return true;
}

//----------------------------------------------------------------------------------------------------
void Game::InitFrame()
{
    Time::GetInstance().StartUpdate();
}

//----------------------------------------------------------------------------------------------------
// game loop
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

    ret = Render();
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
    bool ret = m_systemManager->ShutDown();
    if (!ret) {
        return false;
    }

#ifdef _DRAW
    ret = m_resourceManager->ShutDown();
    if (!ret) {
        return false;
    }
#endif

    ret = m_fsm->ShutDown();
    if (!ret) {
        return false;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PreUpdate()
{
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

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::Update()
{
    bool ret = m_systemManager->Update();
    if (!ret) {
        return false;
    }

    ret = m_fsm->Update();
    if (!ret) {
        return false;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::PostUpdate()
{
    bool ret = m_systemManager->PostUpdate();
    if (!ret) {
        return false;
    }

    ret = m_fsm->PostUpdate();
    if (!ret) {
        return false;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::Render()
{
    bool ret = m_systemManager->Render();
    if (!ret) {
        return false;
    }

    ret = m_fsm->Render();
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
