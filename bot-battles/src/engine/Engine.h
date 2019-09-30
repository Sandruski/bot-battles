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

		ModuleWindow& GetWindow() { return *m_window; }
		ModuleRenderer& GetRenderer() { return *m_renderer; }
		ResourceManager& GetResourceManager() { return *m_resourceManager; }
		ModuleTextureImporter& GetTextureImporter() { return *m_textureImporter; }

		const char* GetName() const { return m_configuration.name; }

	private:
		bool LateUpdate();
		bool Draw();

	public:
		std::shared_ptr<ResourceTexture> m_resourceTexture;

	private:
		EngineConfiguration m_configuration;

		std::unique_ptr<ModuleWindow> m_window;
		std::unique_ptr<ModuleRenderer> m_renderer;
		std::unique_ptr<ResourceManager> m_resourceManager;
		std::unique_ptr<ModuleTextureImporter> m_textureImporter;

		bool m_isInitOk;
		bool m_isActive;
	};

	extern Engine* g_engine;
}

#endif
