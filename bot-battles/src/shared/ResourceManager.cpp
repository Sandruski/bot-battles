#include "ResourceManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ResourceManager::ResourceManager()
    : m_resources()
    , m_availableResources()
{
    m_resources.reserve(MAX_RESOURCES);

    for (U32 i = 0; i < MAX_RESOURCES; ++i) {
        m_availableResources.push(i);
    }
}

//----------------------------------------------------------------------------------------------------
bool ResourceManager::ShutDown()
{
    RemoveAllResources();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ResourceManager::RemoveAllResources()
{
    m_resources.clear();
}
}
