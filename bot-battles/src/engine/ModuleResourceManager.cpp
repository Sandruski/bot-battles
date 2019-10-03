#include "ModuleResourceManager.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	const char* ModuleResourceManager::GetName()
	{
		return "ResourceManager";
	}

	//----------------------------------------------------------------------------------------------------
	ResourceID ModuleResourceManager::GenerateID()
	{
		static ResourceID currentID = 0;
		++currentID;
		return currentID;
	}

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
		RemoveAllResources();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleResourceManager::RemoveAllResources()
	{
		m_resources.clear();
	}
}
