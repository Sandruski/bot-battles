#include "ModuleResourceManager.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	ResourceManager::ResourceManager() :
		m_resources()
	{
	}

	//----------------------------------------------------------------------------------------------------
	ResourceManager::~ResourceManager()
	{
	}

	//----------------------------------------------------------------------------------------------------
	const char* ResourceManager::GetName() const
	{
		return "ResourceManager";
	}

	//----------------------------------------------------------------------------------------------------
	bool ResourceManager::StartUp()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ResourceManager::ShutDown()
	{
		RemoveAll();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ResourceManager::RemoveAll()
	{
		m_resources.clear();
	}
}
