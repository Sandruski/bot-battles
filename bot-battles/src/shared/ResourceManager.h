#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "ResourceDefs.h"

namespace sand {

class Resource;

//----------------------------------------------------------------------------------------------------
class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    bool ShutDown();

    template <class T>
    std::shared_ptr<T> AddResource(const char* file, const char* dir, bool hasPath);
    template <class T>
    U32 RemoveResource(U32 id);
    void RemoveAllResources();

    template <class T>
    std::shared_ptr<T> GetResourceByID(U32 id);
    template <class T>
    std::shared_ptr<T> GetResourceByFile(const char* file);

private:
    std::unordered_map<U32, std::shared_ptr<Resource>> m_resources;
    std::queue<U32> m_availableResources;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::shared_ptr<T> ResourceManager::AddResource(const char* file, const char* dir, bool hasPath)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");
    assert(file != nullptr && dir != nullptr);

    std::shared_ptr<T> resource = hasPath ? GetResourceByFile<T>(file) : nullptr;
    if (resource != nullptr) {
        resource->IncreaseReferences();
        return resource;
    }

    U32 id = m_availableResources.front();
    m_availableResources.pop();

    resource = std::make_shared<T>(id, file, dir);

    const bool isLoaded = resource->Load();
    if (!isLoaded) {
        ELOG("Resource %s%s could not be loaded", dir, file);
        return nullptr;
    }

    auto inserted = m_resources.insert(std::make_pair(id, resource));
    if (!inserted.second) {
        ELOG("Resource %s%s could not be inserted", dir, file);
        return std::static_pointer_cast<T>(inserted.first->second);
    }

    return resource;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline U32 ResourceManager::RemoveResource(U32 id)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");
    assert(id < INVALID_RESOURCE);

    std::shared_ptr<T> resource = GetResourceByID<T>(id);
    if (resource == nullptr) {
        return -1;
    }

    resource->DecreaseReferences();
    if (!resource->HasReferences()) {
        resource->UnLoad();
        m_resources.erase(id);

        m_availableResources.push(id);

        return 0;
    }

    return resource->GetReferences();
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::shared_ptr<T> ResourceManager::GetResourceByID(U32 id)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");
    assert(id < INVALID_RESOURCE);

    auto it = m_resources.find(id);
    if (it == m_resources.end()) {
        WLOG("The resource %u does not exist!", id);
        return nullptr;
    }

    return std::static_pointer_cast<T>((*it).second);
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::shared_ptr<T> ResourceManager::GetResourceByFile(const char* file)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");
    assert(file != nullptr);

    for (auto resource : m_resources) {
        if (COMPARE_STRINGS(resource.second->GetFile(), file)) {
            return std::static_pointer_cast<T>(resource.second);
        }
    }

    WLOG("The resource %s does not exist!", file);
    return nullptr;
}
}

#endif
