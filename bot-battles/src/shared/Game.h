#ifndef __GAME_H__
#define __GAME_H__

namespace sand {

struct Config;

class EntityManager;
class ComponentManager;
class SystemManager;
class FSM;
class LinkingContext;
class ResourceManager;
class FileSystem;

class TextureImporter;
class MeshImporter;
class ShaderImporter;
class MapImporter;

struct WindowComponent;
struct RendererComponent;
struct EventComponent;
struct GuiComponent;
struct PhysicsComponent;
struct MapComponent;
struct MainMenuComponent;
struct GameplayComponent;
struct ScoreboardComponent;

//----------------------------------------------------------------------------------------------------
class Game {
public:
    Game();
    virtual ~Game() = default;

    virtual bool Init();
    void InitFrame();
    virtual bool DoFrame();
    void EndFrame();
    bool End();

    Config& GetConfig() { return *m_config; }
    EntityManager& GetEntityManager() { return *m_entityManager; }
    ComponentManager& GetComponentManager() { return *m_componentManager; }
    SystemManager& GetSystemManager() { return *m_systemManager; }
    FSM& GetFSM() { return *m_fsm; }
    LinkingContext& GetLinkingContext() { return *m_linkingContext; }
    ResourceManager& GetResourceManager() { return *m_resourceManager; }
    FileSystem& GetFileSystem() { return *m_fileSystem; }

    TextureImporter& GetTextureImporter() { return *m_textureImporter; }
    MeshImporter& GetMeshImporter() { return *m_meshImporter; }
    ShaderImporter& GetShaderImporter() { return *m_shaderImporter; }
    MapImporter& GetMapImporter() { return *m_mapImporter; }

    std::weak_ptr<WindowComponent> GetWindowComponent() { return std::weak_ptr(m_windowComponent); }
    std::weak_ptr<RendererComponent> GetRendererComponent() { return std::weak_ptr(m_rendererComponent); }
    std::weak_ptr<EventComponent> GetEventComponent() { return std::weak_ptr(m_EventComponent); }
    std::weak_ptr<GuiComponent> GetGuiComponent() { return std::weak_ptr(m_guiComponent); }
    std::weak_ptr<PhysicsComponent> GetPhysicsComponent() { return std::weak_ptr(m_physicsComponent); }
    std::weak_ptr<MapComponent> GetMapComponent() { return std::weak_ptr(m_mapComponent); }
    std::weak_ptr<MainMenuComponent> GetMainMenuComponent() { return std::weak_ptr(m_mainMenuComponent); }
    std::weak_ptr<GameplayComponent> GetGameplayComponent() { return std::weak_ptr(m_gameplayComponent); }
    std::weak_ptr<ScoreboardComponent> GetScoreboardComponent() { return std::weak_ptr(m_scoreboardComponent); }

private:
    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool PreRender();
    bool Render();
    bool DebugRender();
    bool RenderGui();
    bool PostRender();

public:
    bool m_isRunning;

protected:
    std::shared_ptr<Config> m_config;

    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<ComponentManager> m_componentManager;
    std::shared_ptr<SystemManager> m_systemManager;
    std::shared_ptr<FSM> m_fsm;
    std::shared_ptr<LinkingContext> m_linkingContext;
    std::shared_ptr<ResourceManager> m_resourceManager;
    std::shared_ptr<FileSystem> m_fileSystem;

    std::shared_ptr<TextureImporter> m_textureImporter;
    std::shared_ptr<MeshImporter> m_meshImporter;
    std::shared_ptr<ShaderImporter> m_shaderImporter;
    std::shared_ptr<MapImporter> m_mapImporter;

    std::shared_ptr<WindowComponent> m_windowComponent;
    std::shared_ptr<RendererComponent> m_rendererComponent;
    std::shared_ptr<EventComponent> m_EventComponent;
    std::shared_ptr<GuiComponent> m_guiComponent;
    std::shared_ptr<PhysicsComponent> m_physicsComponent;
    std::shared_ptr<MapComponent> m_mapComponent;
    std::shared_ptr<MainMenuComponent> m_mainMenuComponent;
    std::shared_ptr<GameplayComponent> m_gameplayComponent;
    std::shared_ptr<ScoreboardComponent> m_scoreboardComponent;
};

extern Game* g_game;
}

#endif
