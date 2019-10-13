#include "Game.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"
#include "ResourceManager.h"
#include "FSM.h"
#include "TextureImporter.h"

#include "WindowSystem.h"
#include "RendererSystem.h"
#include "InputSystem.h"

#include "SingletonInputComponent.h"
#include "SingletonRendererComponent.h"
#include "SingletonWindowComponent.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"

#include <SDL.h>

namespace sand {

//----------------------------------------------------------------------------------------------------
Game::Game(const GameConfiguration& configuration)
    : m_configuration(configuration)
    , m_isRunning(false)
{
    m_entityManager = std::make_unique<EntityManager>();
    m_componentManager = std::make_unique<ComponentManager>();
    m_systemManager = std::make_unique<SystemManager>();
	m_resourceManager = std::make_unique<ResourceManager>();
	m_fsm = std::make_unique<FSM>();
	m_textureImporter = std::make_unique<TextureImporter>();

	m_singletonInputComponent = std::make_shared<SingletonInputComponent>();
	m_singletonRendererComponent = std::make_shared<SingletonRendererComponent>();
	m_singletonWindowComponent = std::make_shared<SingletonWindowComponent>();
}

//----------------------------------------------------------------------------------------------------
Game::~Game()
{
}

//----------------------------------------------------------------------------------------------------
bool Game::Init()
{
	bool ret = m_systemManager->RegisterSystem<WindowSystem>();
	if (!ret)
	{
		return false;
	}
	ret = m_systemManager->RegisterSystem<RendererSystem>();
	if (!ret)
	{
		return false;
	}
	ret = m_systemManager->RegisterSystem<InputSystem>();
	if (!ret)
	{
		return false;
	}

	ret = m_componentManager->RegisterComponent<TransformComponent>();
	if (!ret)
	{
		return false;
	}
	ret = m_componentManager->RegisterComponent<SpriteComponent>();
	if (!ret)
	{
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
    bool ret = m_systemManager->PreUpdate(23);
	if (!ret) {
		return false;
	}
		
	ret = m_systemManager->Update(23);
    if (!ret) {
        return false;
    }

    ret = m_systemManager->PostUpdate(23);
    if (!ret) {
        return false;
    }

    ret = m_systemManager->Render();
    if (!ret) {
        return false;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool Game::End()
{
    bool ret = m_systemManager->ShutDown();
    if (!ret) {
        return false;
    }

    m_resourceManager->ShutDown();
    if (!ret) {
        return false;
    }

    m_fsm->ShutDown();
    if (!ret) {
        return false;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
GameConfiguration::GameConfiguration()
    : name(nullptr)
    , StatesSetup(nullptr)
{
}

//----------------------------------------------------------------------------------------------------
GameConfiguration::GameConfiguration(const char* name, std::function<void()> StatesSetup)
    : name(name)
    , StatesSetup(StatesSetup)
{
}

//----------------------------------------------------------------------------------------------------
GameConfiguration::GameConfiguration(const GameConfiguration& configuration)
    : name(configuration.name)
    , StatesSetup(configuration.StatesSetup)
{
}
}
