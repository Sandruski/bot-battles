#ifndef __GAME_H__
#define __GAME_H__

#ifdef _DRAW
#include "GuiComponent.h"
#include "MeshImporter.h"
#include "RendererComponent.h"
#include "ShaderImporter.h"
#include "TextureImporter.h"
#include "WindowComponent.h"
#endif
#include "FileSystem.h"
#include "GameplayComponent.h"
#include "MapImporter.h"
#include "ResourceManager.h"

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
    ShaderImporter& GetShaderImporter()
    {
        return m_shaderImporter;
    }
    TextureImporter& GetTextureImporter()
    {
        return m_textureImporter;
    }
    MeshImporter& GetMeshImporter()
    {
        return m_meshImporter;
    }
    WindowComponent& GetWindowComponent() { return m_windowComponent; }
    RendererComponent& GetRendererComponent() { return m_rendererComponent; }
    GuiComponent& GetGuiComponent() { return m_guiComponent; }
#endif
    FileSystem& GetFileSystem()
    {
        return m_fileSystem;
    }
    ResourceManager& GetResourceManager() { return m_resourceManager; }
    MapImporter& GetMapImporter()
    {
        return m_mapImporter;
    }
    GameplayComponent& GetGameplayComponent()
    {
        return m_gameplayComponent;
    }

protected:
    virtual bool Update();

private:
    bool PreUpdate();
    bool PostUpdate();
    bool PreRender();
    bool Render();
    bool DebugRender();
    bool RenderGui();
    bool PostRender();

protected:
    std::shared_ptr<Config> m_config;

    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<ComponentManager> m_componentManager;
    std::shared_ptr<SystemManager> m_systemManager;
    std::shared_ptr<FSM> m_fsm;
    std::shared_ptr<LinkingContext> m_linkingContext;
#ifdef _DRAW
    ShaderImporter m_shaderImporter;
    TextureImporter m_textureImporter;
    MeshImporter m_meshImporter;
    WindowComponent m_windowComponent;
    RendererComponent m_rendererComponent;
    GuiComponent m_guiComponent;
#endif
    FileSystem m_fileSystem;
    ResourceManager m_resourceManager;
    MapImporter m_mapImporter;
    GameplayComponent m_gameplayComponent;

    bool m_isRunning;
};

extern Game* g_game;
}

#endif
