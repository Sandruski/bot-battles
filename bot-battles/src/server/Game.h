#ifndef __GAME_H__
#define __GAME_H__

#include "Timer.h"

// uncomment to disable assert()
// #define NDEBUG

namespace sand {

class EntityManager;
class ComponentManager;
class SystemManager;
class ResourceManager;
class FSM;

class LinkingContext;

struct SingletonServerComponent;

//----------------------------------------------------------------------------------------------------
struct GameConfiguration {
    GameConfiguration();
    GameConfiguration(const char* name, std::function<void()> StatesSetup);
    GameConfiguration(const GameConfiguration& configuration);

    const char* m_name;
    std::function<void()> StatesSetup;
};

//----------------------------------------------------------------------------------------------------
// single point of contact
class Game {
public:
    Game(const GameConfiguration& configuration);
    virtual ~Game();

    bool Init();
    bool Update();
    bool End();

    EntityManager& GetEntityManager() const { return *m_entityManager; }
    ComponentManager& GetComponentManager() const { return *m_componentManager; }
    SystemManager& GetSystemManager() const { return *m_systemManager; }
    ResourceManager& GetResourceManager() const { return *m_resourceManager; }
    FSM& GetFSM() const { return *m_fsm; }

    LinkingContext& GetLinkingContext() const { return *m_linkingContext; }

    std::shared_ptr<SingletonServerComponent> GetSingletonServerComponent() const { return m_singletonServerComponent; }

    const char* GetName() const { return m_configuration.m_name; }

private:
    GameConfiguration m_configuration;

    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<ComponentManager> m_componentManager;
    std::shared_ptr<SystemManager> m_systemManager;
    std::shared_ptr<ResourceManager> m_resourceManager;
    std::shared_ptr<FSM> m_fsm;

    std::unique_ptr<LinkingContext> m_linkingContext;

    std::shared_ptr<SingletonServerComponent> m_singletonServerComponent;

    Timer m_timer;
    F64 m_lastFrameMs;
    F64 m_desiredFramerate;
    F64 m_fps;
    F64 m_dt;

    bool m_isRunning;
};

extern Game* g_game;
}

#endif
