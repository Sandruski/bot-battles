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
	const char* ModuleResourceManager::GetName() const
	{
		return "ResourceManager";
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleResourceManager::RemoveAll()
	{
		m_resources.clear();
	}
}
