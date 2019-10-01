#include "ModuleResourceManager.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleResourceManager::ModuleResourceManager() : Module(true),
		m_resources()
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleResourceManager::~ModuleResourceManager()
	{
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleResourceManager::GetName() const
	{
		return "ResourceManager";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleResourceManager::StartUp()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleResourceManager::ShutDown()
	{
		RemoveAll();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleResourceManager::RemoveAll()
	{
		m_resources.clear();
	}
}
