#ifndef __GAME_H__
#define __GAME_H__

#include "FSM.h"
#include "FontImporter.h"
#include "LinkingContext.h"
#include "ResourceManager.h"
#include "TextureImporter.h"
#ifdef _DRAW
#include "RendererComponent.h"
#include "WindowComponent.h"
#endif

namespace sand {

struct Config;
class EntityManager;
class ComponentManager;
class SystemManager;

//----------------------------------------------------------------------------------------------------
struct GameConfiguration {
    GameConfiguration();
    GameConfiguration(std::function<void()> StatesSetup);
    GameConfiguration(const GameConfiguration& configuration);

    std::function<void()> StatesSetup;
};

//----------------------------------------------------------------------------------------------------
class Game {
public:
    Game();
    virtual ~Game() = default;

    virtual bool Init();
    void InitFrame();
    bool DoFrame();
    void EndFrame();
    bool End();

    Config& GetConfig() { return *m_config; }
    EntityManager& GetEntityManager() { return *m_entityManager; }
    ComponentManager& GetComponentManager() { return *m_componentManager; }
    SystemManager& GetSystemManager() { return *m_systemManager; }
    LinkingContext& GetLinkingContext() { return m_linkingContext; }
    FSM& GetFSM() { return m_fsm; } // TODO: remove this!
#ifdef _DRAW
    FontImporter& GetFontImporter()
    {
        return m_fontImporter;
    }
    TextureImporter& GetTextureImporter() { return m_textureImporter; }
    ResourceManager& GetResourceManager() { return m_resourceManager; }

    WindowComponent& GetWindowComponent() { return m_windowComponent; }
    RendererComponent& GetRendererComponent() { return m_rendererComponent; }
#endif

private:
    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool PreRender();
    bool Render();
    bool PostRender();

protected:
    std::shared_ptr<Config> m_config;
    GameConfiguration m_configuration;

    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<ComponentManager> m_componentManager;
    std::shared_ptr<SystemManager> m_systemManager;

    LinkingContext m_linkingContext;
    FSM m_fsm; // TODO: remove this!
#ifdef _DRAW
    FontImporter m_fontImporter;
    TextureImporter m_textureImporter;
    ResourceManager m_resourceManager;

    WindowComponent m_windowComponent;
    RendererComponent m_rendererComponent;
#endif

    bool m_isRunning;
};

extern Game* g_game;
}

#endif
