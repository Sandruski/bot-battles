#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <memory>

// uncomment to disable assert()
// #define NDEBUG

namespace sand
{

	class ModuleWindow;
	class ModuleRenderer;
	class ModuleTextureImporter;
	class ModuleInput;
	class ResourceManager;
	class ResourceTexture;

	//----------------------------------------------------------------------------------------------------
	struct EngineConfiguration 
	{
		EngineConfiguration() : name(nullptr) {}
		EngineConfiguration(const char* name) : name(name) {}

		const char* name;
	};

	//----------------------------------------------------------------------------------------------------
	// single point of contact
	class Engine
	{
	public:
		Engine(const char* name);
		virtual ~Engine();

		bool Init();
		bool Update();
		bool End();

		ModuleWindow& GetWindow() const { return *m_window; }
		ModuleRenderer& GetRenderer() const { return *m_renderer; }
		ModuleTextureImporter& GetTextureImporter() const { return *m_textureImporter; }
		ModuleInput& GetInput() const { return *m_input; }
		ResourceManager& GetResourceManager() const { return *m_resourceManager; }

		const char* GetName() const { return m_configuration.name; }
		bool IsExit() const { return m_exit; }

	private:
		bool LateUpdate();
		bool Draw();

	public:
		std::shared_ptr<ResourceTexture> m_resourceTexture;

	private:
		EngineConfiguration m_configuration;

		std::unique_ptr<ModuleWindow> m_window;
		std::unique_ptr<ModuleRenderer> m_renderer;
		std::unique_ptr<ModuleTextureImporter> m_textureImporter;
		std::unique_ptr<ModuleInput> m_input;
		std::unique_ptr<ResourceManager> m_resourceManager;

		bool m_isInitOk;
		bool m_exit;
	};

	extern Engine* g_engine;
}

#endif
