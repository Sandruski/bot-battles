#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

// TODO: interface IComponent struct where all components derive from. Create() method implementation for resources
// NOTE: entities can only have one component of each type

namespace sand {

class ComponentManager;
class Component;

//----------------------------------------------------------------------------------------------------
class IComponentArray {
public:
    virtual bool RemoveComponent(Entity entity, ComponentManager& componentManager) = 0;
    virtual void KillComponent(Entity entity) = 0;
};

//----------------------------------------------------------------------------------------------------
template <class T>
class ComponentArray : public IComponentArray {
public:
    ComponentArray();
    ~ComponentArray() = default;

    bool RemoveComponent(Entity entity, ComponentManager& componentManager) override;
    void KillComponent(Entity entity) override;

    std::weak_ptr<T> AddComponent(Entity entity, ComponentManager& componentManager);
    std::weak_ptr<T> GetComponent(Entity entity);

private:
    std::array<std::shared_ptr<T>, MAX_ENTITIES> m_components;
    std::unordered_map<Entity, U32> m_entitiesToComponents;
    U32 m_componentsSize;
};

//----------------------------------------------------------------------------------------------------
class ComponentManager : public Subject, public Observer {
public:
    ComponentManager();
    ~ComponentManager() override = default;

    bool PreUpdate();

    void OnNotify(const Event& event) override;

    template <class T>
    bool RegisterComponent();
    template <class T>
    bool DeRegisterComponent();

    template <class T>
    std::weak_ptr<T> AddComponent(Entity entity);
    template <class T>
    std::weak_ptr<T> GetComponent(Entity entity);
    template <class T>
    bool RemoveComponent(Entity entity);

    void OnEntityRemoved(Entity entity);
    void OnComponentRemoved(ComponentType componentType, Entity entity);

private:
    std::array<std::shared_ptr<IComponentArray>, MAX_COMPONENTS> m_componentArrays;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline ComponentArray<T>::ComponentArray()
    : m_components()
    , m_entitiesToComponents()
    , m_componentsSize(0)
{
    m_components.fill(NULL);
}

//----------------------------------------------------------------------------------------------------
template <class T>
bool ComponentArray<T>::RemoveComponent(Entity entity, ComponentManager& componentManager)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        WLOG("Entity %u does not have the component!", entity);
        return false;
    }

    Event newEvent;
    newEvent.eventType = EventType::COMPONENT_REMOVED;
    newEvent.component.componentType = T::GetType();
    newEvent.component.entity = entity;
    componentManager.PushEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline void ComponentArray<T>::KillComponent(Entity entity)
{
    U32 componentIndex = m_entitiesToComponents.at(entity);
    U32 lastComponentIndex = m_componentsSize - 1;
    m_components.at(componentIndex) = m_components.at(lastComponentIndex);
    m_components.at(lastComponentIndex) = nullptr;

    auto lastEntityToComponent = std::find_if(m_entitiesToComponents.begin(),
        m_entitiesToComponents.end(),
        [lastComponentIndex](const auto& value) {
            return value.second == lastComponentIndex;
        });
    assert(lastEntityToComponent != m_entitiesToComponents.end());

    m_entitiesToComponents.insert(std::make_pair(lastEntityToComponent->first, componentIndex));
    m_entitiesToComponents.erase(entity);

    --m_componentsSize;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> ComponentArray<T>::AddComponent(Entity entity, ComponentManager& componentManager)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent != m_entitiesToComponents.end()) {
        WLOG("Entity %u already has the component!", entity);
        return m_components.at(entityToComponent->second);
    }

    U32 componentIndex = m_componentsSize;

    std::shared_ptr<T> component = T::Instantiate();
    m_components.at(componentIndex) = component;
    m_entitiesToComponents.insert(std::make_pair(entity, componentIndex));

    ++m_componentsSize;

    Event newEvent;
    newEvent.eventType = EventType::COMPONENT_ADDED;
    newEvent.component.componentType = T::GetType();
    newEvent.component.entity = entity;
    componentManager.PushEvent(newEvent);

    return std::weak_ptr(component);
}

//----------------------------------------------------------------------------------------------------
template <class T>
std::weak_ptr<T> ComponentArray<T>::GetComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        WLOG("Entity %u does not have the component!", entity);
        return std::weak_ptr<T>();
    }

    std::shared_ptr component = m_components.at(entityToComponent->second);

    return std::weak_ptr(component);
}

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
std::weak_ptr<T> ComponentManager::AddComponent(Entity entity)
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    assert(entity < INVALID_ENTITY);
    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[componentIndex])->AddComponent(entity, *this);
}

//----------------------------------------------------------------------------------------------------
template <class T>
std::weak_ptr<T> ComponentManager::GetComponent(Entity entity)
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    assert(entity < INVALID_ENTITY);
    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[componentIndex])->GetComponent(entity);
}

//----------------------------------------------------------------------------------------------------
template <class T>
bool ComponentManager::RemoveComponent(Entity entity)
{
    static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    assert(entity < INVALID_ENTITY);
    ComponentType componentType = T::GetType();
    assert(componentType < ComponentType::COUNT);
    std::size_t componentIndex = static_cast<std::size_t>(componentType);
    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[componentIndex])->RemoveComponent(entity);
}
}

#endif
