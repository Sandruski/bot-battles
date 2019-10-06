#ifndef __GAME_H__
#define __GAME_H__

#include <memory>
#include <functional>
#include <vector>

// uncomment to disable assert()
// #define NDEBUG

namespace sand
{

	class ModuleWindow;
	class ModuleTextureImporter;
	class ModuleResourceManager;
	class ModuleFSM;

	class EntityManager;
	class ComponentManager;

	struct RendererComponent;
	struct InputComponent;

	class System;

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
		ModuleResourceManager& GetResourceManager() const { return *m_resourceManager; }
		ModuleFSM& GetFSM() const { return *m_fsm; }

		EntityManager& GetEntityManager() const { return *m_entityManager; }
		ComponentManager& GetComponentManager() const { return *m_componentManager; }

		RendererComponent& GetRendererComponent() const { return *m_rendererComponent; }
		InputComponent& GetInputComponent() const { return *m_inputComponent; }

		const char* GetName() const { return m_configuration.name; }

	private:
		bool LateUpdate();
		bool Render();

	private:
		GameConfiguration m_configuration;

		std::unique_ptr<ModuleWindow> m_window;
		std::unique_ptr<ModuleTextureImporter> m_textureImporter;
		std::unique_ptr<ModuleResourceManager> m_resourceManager;
		std::unique_ptr<ModuleFSM> m_fsm;

		std::unique_ptr<EntityManager> m_entityManager;
		std::unique_ptr<ComponentManager> m_componentManager;

		std::unique_ptr<RendererComponent> m_rendererComponent;
		std::unique_ptr<InputComponent> m_inputComponent;

		std::vector<std::unique_ptr<System>> m_systems;

		bool m_isInitOk;
		bool m_isRunning;
	};

	extern Game* g_game;
}

#endif
