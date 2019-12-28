#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "ResourceDefs.h"

namespace sand {

class Resource;

//----------------------------------------------------------------------------------------------------
class ResourceManager {
public:
    ResourceManager();

    bool ShutDown();

    template <class T>
    std::weak_ptr<T> AddResource(const char* file, const char* dir, bool hasPath);
    template <class T>
    U32 RemoveResource(U32 id);
    void RemoveAllResources();

    template <class T>
    std::weak_ptr<T> GetResourceByID(U32 id);
    template <class T>
    std::weak_ptr<T> GetResourceByFile(const char* file);

private:
    std::unordered_map<U32, std::shared_ptr<Resource>> m_resources;
    std::queue<U32> m_availableResources;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> ResourceManager::AddResource(const char* file, const char* dir, bool hasPath)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");

    assert(file != nullptr && dir != nullptr);
    if (hasPath) {
        std::weak_ptr<T> resource = GetResourceByFile<T>(file);
        if (!resource.expired()) {
            resource.lock()->IncreaseReferences();
            return resource;
        }
    }

    U32 id = m_availableResources.front();
    m_availableResources.pop();

    std::shared_ptr<T> resource = std::make_shared<T>(id, file, dir);

    const bool isLoaded = resource->Load();
    if (!isLoaded) {
        ELOG("Resource %s%s could not be loaded", dir, file);
        return std::weak_ptr<T>();
    }

    m_resources.insert(std::make_pair(id, resource));

    return std::weak_ptr<T>(resource);
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
inline std::weak_ptr<T> ResourceManager::GetResourceByID(U32 id)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");

    assert(id < INVALID_RESOURCE);
    auto it = m_resources.find(id);
    if (it == m_resources.end()) {
        WLOG("Resource %u does not exist!", id);
        return std::weak_ptr<T>();
    }

    std::shared_ptr<Resource> resource = (*it).second;
    return std::weak_ptr<T>(std::static_pointer_cast<T>(resource));
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> ResourceManager::GetResourceByFile(const char* file)
{
    static_assert(std::is_base_of<Resource, T>::value, "T is not derived from Resource");

    assert(file != nullptr);
    for (const auto& pair : m_resources) {
        std::shared_ptr<Resource> resource = pair.second;
        if (COMPARE_STRINGS(resource->GetFile(), file)) {
            return std::weak_ptr<T>(std::static_pointer_cast<T>(resource));
        }
    }

    WLOG("Resource %s does not exist!", file);
    return std::weak_ptr<T>();
}
}

#endif
