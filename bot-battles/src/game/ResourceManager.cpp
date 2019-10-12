#include "ResourceManager.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ResourceManager::ResourceManager() :
		m_resources(),
		m_availableResources()
	{
		m_resources.reserve(MAX_RESOURCES);

		for (U32 i = 0; i < MAX_RESOURCES; ++i) {
			m_availableResources.push(i);
		}
	}

	//----------------------------------------------------------------------------------------------------
	ResourceManager::~ResourceManager()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ResourceManager::ShutDown()
	{
		m_resources.clear();

		return true;
	}
}
