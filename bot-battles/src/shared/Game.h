#ifndef __GAME_H__
#define __GAME_H__

#ifdef _DRAW
#include "FontImporter.h"
#include "ResourceManager.h"
#include "TextureImporter.h"

#include "RendererComponent.h"
#include "WindowComponent.h"
#endif
#include "GameplayComponent.h"

namespace sand {

struct Config;

class EntityManager;
class ComponentManager;
class SystemManager;
class FSM;
class LinkingContext;

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
    FSM& GetFSM() { return *m_fsm; }
    LinkingContext& GetLinkingContext() { return *m_linkingContext; }
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
    GameplayComponent& GetGameplayComponent() { return m_gameplayComponent; }

protected:
    virtual bool Update();

private:
    bool PreUpdate();
    bool PostUpdate();
    bool PreRender();
    bool Render();
    bool DebugRender();
    bool PostRender();

protected:
    std::shared_ptr<Config> m_config;

    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<ComponentManager> m_componentManager;
    std::shared_ptr<SystemManager> m_systemManager;
    std::shared_ptr<FSM> m_fsm;

    std::shared_ptr<LinkingContext> m_linkingContext;
#ifdef _DRAW
    FontImporter m_fontImporter;
    TextureImporter m_textureImporter;
    ResourceManager m_resourceManager;

    WindowComponent m_windowComponent;
    RendererComponent m_rendererComponent;
#endif
    GameplayComponent m_gameplayComponent;

    bool m_isRunning;
};

extern Game* g_game;
}

#endif
