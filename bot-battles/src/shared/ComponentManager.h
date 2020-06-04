#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

namespace sand {

struct Component;

//----------------------------------------------------------------------------------------------------
class IComponentArray {
    friend class ComponentManager;

public:
    virtual ~IComponentArray() = default;

    virtual std::weak_ptr<Component> AddComponent(Entity entity) = 0;
    virtual std::weak_ptr<Component> GetComponent(Entity entity) = 0;
    virtual std::vector<std::pair<Entity, std::weak_ptr<Component>>> GetComponents() = 0;
    virtual bool RemoveComponent(Entity entity) = 0;

private:
    virtual bool KillComponent(Entity entity) = 0;
};

//----------------------------------------------------------------------------------------------------
template <class T>
class ComponentArray : public IComponentArray {
public:
    ComponentArray();

    std::weak_ptr<Component> AddComponent(Entity entity) override;
    std::weak_ptr<Component> GetComponent(Entity entity) override;
    std::vector<std::pair<Entity, std::weak_ptr<Component>>> GetComponents() override;
    bool RemoveComponent(Entity entity) override;

private:
    bool KillComponent(Entity entity) override;

private:
    std::array<std::shared_ptr<T>, MAX_ENTITIES> m_components;
    std::unordered_map<Entity, U32> m_entitiesToComponents;
    U32 m_componentsSize;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline ComponentArray<T>::ComponentArray()
    : m_components()
    , m_entitiesToComponents()
    , m_componentsSize(0)
{
    m_components.fill(nullptr);
    m_entitiesToComponents.reserve(MAX_ENTITIES);
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentArray<T>::RemoveComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        WLOG("Entity %u does not have the component!", entity);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<Component> ComponentArray<T>::AddComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent != m_entitiesToComponents.end()) {
        WLOG("Entity %u already has the component!", entity);
        std::shared_ptr<T> component = m_components.at(entityToComponent->second);
        return std::weak_ptr<Component>(component);
    }

    U32 componentIndex = m_componentsSize;

    std::shared_ptr<T> component = std::make_shared<T>();
    m_components.at(componentIndex) = component;
    m_entitiesToComponents.insert(std::make_pair(entity, componentIndex));

    ++m_componentsSize;

    return std::weak_ptr<Component>(component);
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<Component> ComponentArray<T>::GetComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        return std::weak_ptr<Component>();
    }

    std::shared_ptr<T> component = m_components.at(entityToComponent->second);
    return std::weak_ptr<Component>(component);
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::vector<std::pair<Entity, std::weak_ptr<Component>>> ComponentArray<T>::GetComponents()
{
    std::vector<std::pair<Entity, std::weak_ptr<Component>>> components;
    for (const auto& pair : m_entitiesToComponents) {
        Entity entity = pair.first;
        std::shared_ptr<T> component = m_components.at(pair.second);
        components.emplace_back(std::make_pair(entity, std::weak_ptr<Component>(component)));
    }
    return components;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentArray<T>::KillComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        return false;
    }

    U32 componentIndex = entityToComponent->second;
    U32 lastComponentIndex = m_componentsSize - 1;
    m_components.at(componentIndex) = m_components.at(lastComponentIndex);
    m_components.at(lastComponentIndex) = nullptr;

    auto lastEntityToComponent = std::find_if(m_entitiesToComponents.begin(),
        m_entitiesToComponents.end(),
        [lastComponentIndex](const auto& value) {
            return value.second == lastComponentIndex;
        });
    assert(lastEntityToComponent != m_entitiesToComponents.end());

    m_entitiesToComponents.at(lastEntityToComponent->first) = componentIndex;
    m_entitiesToComponents.erase(entity);

    --m_componentsSize;

    return true;
}

//----------------------------------------------------------------------------------------------------
class ComponentManager : public Subject, public Observer, public std::enable_shared_from_this<ComponentManager> {
public:
    ComponentManager();

    void OnNotify(const Event& event) override;

    bool PreUpdate();

    template <class T>
    bool RegisterComponent();
    template <class T>
    bool DeRegisterComponent();

    template <class T>
    std::weak_ptr<T> AddComponent(Entity entity);
    std::weak_ptr<Component> AddComponent(ComponentType componentType, Entity entity);
    template <class T>
    std::weak_ptr<T> GetComponent(Entity entity);
    std::weak_ptr<Component> GetComponent(ComponentType componentType, Entity entity);
    template <class T>
    std::vector<std::pair<Entity, std::weak_ptr<T>>> GetComponents();
    template <class T>
    bool RemoveComponent(Entity entity);
    bool RemoveComponent(ComponentType componentType, Entity entity);

private:
    void OnEntityRemoved(Entity entity);
    void OnComponentRemoved(ComponentType componentType, Entity entity);

private:
    std::array<std::shared_ptr<IComponentArray>, MAX_COMPONENTS> m_componentArrays;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentManager::RegisterComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    std::shared_ptr<IComponentArray> componentArray = m_componentArrays.at(componentIndex);
    if (componentArray != nullptr) {
        WLOG("Component array %u is already registered!", componentIndex);
        return false;
    }

    m_componentArrays.at(componentIndex) = std::make_shared<ComponentArray<T>>();

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentManager::DeRegisterComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    std::shared_ptr<IComponentArray> componentArray = m_componentArrays.at(componentIndex);
    if (componentArray == nullptr) {
        WLOG("Component array %u is not registered!", componentIndex);
        return false;
    }

    m_componentArrays.at(componentIndex) = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> ComponentManager::AddComponent(Entity entity)
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    assert(entity < INVALID_ENTITY);
    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    std::weak_ptr<Component> baseComponent = m_componentArrays.at(componentIndex)->AddComponent(entity);
    std::weak_ptr<T> derivedComponent = std::weak_ptr<T>(std::static_pointer_cast<T>(baseComponent.lock()));
    if (!derivedComponent.expired()) {
        Event newEvent;
        newEvent.eventType = EventType::COMPONENT_ADDED;
        newEvent.component.componentType = T::GetType();
        newEvent.component.entity = entity;
        NotifyEvent(newEvent);
    }

    return derivedComponent;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> ComponentManager::GetComponent(Entity entity)
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    assert(entity < INVALID_ENTITY);
    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    std::weak_ptr<Component> baseComponent = m_componentArrays.at(componentIndex)->GetComponent(entity);
    std::weak_ptr<T> derivedComponent = std::weak_ptr<T>(std::static_pointer_cast<T>(baseComponent.lock()));

    return derivedComponent;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::vector<std::pair<Entity, std::weak_ptr<T>>> ComponentManager::GetComponents()
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    std::vector<std::pair<Entity, std::weak_ptr<Component>>> baseComponents = m_componentArrays.at(componentIndex)->GetComponents();
    std::vector<std::pair<Entity, std::weak_ptr<T>>> derivedComponents;
    for (const auto& pair : baseComponents) {
        Entity entity = pair.first;
        std::weak_ptr<T> derivedComponent = std::weak_ptr<T>(std::static_pointer_cast<T>(pair.second.lock()));
        derivedComponents.emplace_back(std::make_pair(entity, derivedComponent));
    }

    return derivedComponents;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentManager::RemoveComponent(Entity entity)
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    assert(entity < INVALID_ENTITY);
    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    bool ret = m_componentArrays.at(componentIndex)->RemoveComponent(entity);
    if (ret) {
        Event newEvent;
        newEvent.eventType = EventType::COMPONENT_REMOVED;
        newEvent.component.componentType = T::GetType();
        newEvent.component.entity = entity;
        PushEvent(newEvent);
    }

    return ret;
}
}

#endif
