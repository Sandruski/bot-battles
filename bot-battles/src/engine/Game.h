#ifndef __GAME_H__
#define __GAME_H__

#include <memory>
#include <functional>

// uncomment to disable assert()
// #define NDEBUG

namespace sand
{

	class ModuleWindow;
	class ModuleTextureImporter;
	class ModuleInput;
	class ModuleResourceManager;
	class ModuleFSM;

	class EntityManager;
	class ComponentManager;

	class RendererSystem;

	//----------------------------------------------------------------------------------------------------
	struct GameConfiguration
	{
		GameConfiguration();
		GameConfiguration(const char* name, std::function<void()> StatesSetup);
		GameConfiguration(const GameConfiguration& configuration);

		const char* name;
		std::function<void()> StatesSetup;
	};

	//----------------------------------------------------------------------------------------------------
	// single point of contact
	class Game
	{
	public:
		Game(const GameConfiguration& configuration);
		virtual ~Game();

		bool Init();
		bool Update();
		bool End();

		ModuleWindow& GetWindow() const { return *m_window; }
		ModuleTextureImporter& GetTextureImporter() const { return *m_textureImporter; }
		ModuleInput& GetInput() const { return *m_input; }
		ModuleResourceManager& GetResourceManager() const { return *m_resourceManager; }
		ModuleFSM& GetFSM() const { return *m_fsm; }

		EntityManager& GetEntityManager() const { return *m_entityManager; }
		ComponentManager& GetComponentManager() const { return *m_componentManager; }

		RendererSystem& GetRenderer() const { return *m_renderer; }

		const char* GetName() const { return m_configuration.name; }

	private:
		bool LateUpdate();
		bool Render();

	private:
		GameConfiguration m_configuration;

		std::unique_ptr<ModuleWindow> m_window;
		std::unique_ptr<ModuleTextureImporter> m_textureImporter;
		std::unique_ptr<ModuleInput> m_input;
		std::unique_ptr<ModuleResourceManager> m_resourceManager;
		std::unique_ptr<ModuleFSM> m_fsm;

		std::unique_ptr<EntityManager> m_entityManager;
		std::unique_ptr<ComponentManager> m_componentManager;

		std::unique_ptr<RendererSystem> m_renderer;

		bool m_isInitOk;
		bool m_isRunning;
	};

	extern Game* g_game;
}

#endif
