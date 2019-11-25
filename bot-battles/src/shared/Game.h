#ifndef __GAME_H__
#define __GAME_H__

// uncomment to disable assert()
// #define NDEBUG

namespace sand {

class EntityManager;
class ComponentManager;
class SystemManager;
class LinkingContext;
class FSM; // TODO: remove this!
class ResourceManager;
class TextureImporter;
class FontImporter;

#ifdef _DRAW
struct SingletonWindowComponent;
struct SingletonRendererComponent;
#endif

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

    virtual bool Init();
    void InitFrame();
    bool DoFrame();
    void EndFrame();
    bool End();

    EntityManager& GetEntityManager() const { return *m_entityManager; }
    ComponentManager& GetComponentManager() const { return *m_componentManager; }
    SystemManager& GetSystemManager() const { return *m_systemManager; }
    LinkingContext& GetLinkingContext() const { return *m_linkingContext; }
    FSM& GetFSM() const { return *m_fsm; } // TODO: remove this!
#ifdef _DRAW
    ResourceManager& GetResourceManager() const
    {
        return *m_resourceManager;
    }
    TextureImporter& GetTextureImporter() const { return *m_textureImporter; }
	FontImporter& GetFontImporter() const { return *m_fontImporter; }

    std::shared_ptr<SingletonWindowComponent> GetSingletonWindowComponent() const
    {
        return m_singletonWindowComponent;
    }
    std::shared_ptr<SingletonRendererComponent> GetSingletonRendererComponent() const { return m_singletonRendererComponent; }
#endif

    const char* GetName() const
    {
        return m_configuration.m_name;
    }

private:
    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool Render();

protected:
    GameConfiguration m_configuration;

    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<ComponentManager> m_componentManager;
    std::shared_ptr<SystemManager> m_systemManager;
    std::unique_ptr<LinkingContext> m_linkingContext;
    std::shared_ptr<FSM> m_fsm; // TODO: remove this!
    std::shared_ptr<ResourceManager> m_resourceManager;
    std::shared_ptr<TextureImporter> m_textureImporter;
	std::shared_ptr<FontImporter> m_fontImporter;

#ifdef _DRAW
    std::shared_ptr<SingletonWindowComponent> m_singletonWindowComponent;
    std::shared_ptr<SingletonRendererComponent> m_singletonRendererComponent;
#endif

    bool m_isRunning;
};

extern Game* g_game;
}

#endif
