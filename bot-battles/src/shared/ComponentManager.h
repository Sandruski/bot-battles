#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"

#include "Events.h"
#include "Observer.h"
#include "Subject.h"

// TODO: interface IComponent struct where all components derive from. Create() method implementation for resources
// NOTE: entities can only have one component of each type

namespace sand {

//----------------------------------------------------------------------------------------------------
class IComponentArray : public Subject {
public:
    IComponentArray() { }
    virtual ~IComponentArray() { }

    virtual bool PreUpdate(F32 /*dt*/) = 0;

    virtual void OnNotify(const Event& event) = 0;
};

//----------------------------------------------------------------------------------------------------
template <class T>
class ComponentArray : public IComponentArray {
public:
    ComponentArray();
    ~ComponentArray();

    bool PreUpdate(F32 /*dt*/) override;

    std::shared_ptr<T> AddComponent(Entity entity);
    bool RemoveComponent(Entity entity);
    std::shared_ptr<T> GetComponent(Entity entity);
    void KillComponent(Entity entity);

    void OnNotify(const Event& event) override;

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
    memset(&m_components, NULL, MAX_ENTITIES);
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline ComponentArray<T>::~ComponentArray()
{
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentArray<T>::PreUpdate(F32)
{
    NotifyAll();

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::shared_ptr<T> ComponentArray<T>::AddComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent != m_entitiesToComponents.end()) {
        WLOG("The entity %u already has the component!", entity);
        return m_components[entityToComponent->second];
    }

    U32 componentIndex = m_componentsSize;
    ++m_componentsSize;

    m_entitiesToComponents[entity] = componentIndex;

    std::shared_ptr component = std::make_shared<T>();
    m_components[componentIndex] = component;

    Event newEvent;
    newEvent.eventType = EventType::COMPONENT_ADDED;
    newEvent.component.componentType = T::GetType();
    newEvent.component.entity = entity;
    PushEvent(newEvent);

    return component;
}

//----------------------------------------------------------------------------------------------------
template <class T>
bool ComponentArray<T>::RemoveComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        WLOG("The entity %u does not have the component!", entity);
        return false;
    }

    Event newEvent;
    newEvent.eventType = EventType::COMPONENT_REMOVED;
    newEvent.component.componentType = T::GetType();
    newEvent.component.entity = entity;
    PushEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
std::shared_ptr<T> ComponentArray<T>::GetComponent(Entity entity)
{
    auto entityToComponent = m_entitiesToComponents.find(entity);
    if (entityToComponent == m_entitiesToComponents.end()) {
        WLOG("The entity %u does not have the component!", entity);
        return nullptr;
    }

    return m_components[entityToComponent->second];
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline void ComponentArray<T>::KillComponent(Entity entity)
{
    U32 componentIndex = m_entitiesToComponents[entity];
    U32 lastComponentIndex = m_componentsSize - 1;
    m_components[componentIndex] = m_components[lastComponentIndex];
    m_components[lastComponentIndex] = nullptr;

    auto lastEntityToComponent = std::find_if(m_entitiesToComponents.begin(),
        m_entitiesToComponents.end(),
        [lastComponentIndex](const auto& value) {
            return value.second == lastComponentIndex;
        });
    assert(lastEntityToComponent != m_entitiesToComponents.end());

    m_entitiesToComponents[lastEntityToComponent->first] = componentIndex;
    m_entitiesToComponents.erase(entity);

    --m_componentsSize;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline void ComponentArray<T>::OnNotify(const Event& event)
{
    switch (event.eventType) {
    case EventType::COMPONENT_REMOVED: {
        if (event.component.componentType == T::GetType()) {
            KillComponent(event.component.entity);
        }
        break;
    }

    case EventType::ENTITY_REMOVED: {
        RemoveComponent(event.entity.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
class ComponentManager : public Observer {
public:
    ComponentManager();
    ~ComponentManager();

    bool PreUpdate(F32 dt);

    template <class T>
    bool RegisterComponent();
    template <class T>
    bool DeRegisterComponent();

    template <class T>
    std::shared_ptr<T> AddComponent(Entity entity);
    template <class T>
    bool RemoveComponent(Entity entity);
    template <class T>
    std::shared_ptr<T> GetComponent(Entity entity);

    bool AddObserver(std::shared_ptr<Observer> observer);
    bool RemoveObserver(std::shared_ptr<Observer> observer);

    void OnNotify(const Event& event) override;

private:
    std::array<std::shared_ptr<IComponentArray>, static_cast<std::size_t>(ComponentType::COUNT)> m_componentArrays;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentManager::RegisterComponent()
{
    //static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    ComponentType type = T::GetType();
    assert(type != ComponentType::COUNT && type != ComponentType::INVALID);

    auto componentArray = m_componentArrays[static_cast<std::size_t>(type)];
    if (componentArray != nullptr) {
        WLOG("The component array already exists!");
        return false;
    }

    m_componentArrays[static_cast<std::size_t>(type)] = std::make_shared<ComponentArray<T>>();

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool ComponentManager::DeRegisterComponent()
{
    //static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

    ComponentType type = T::GetType();
    assert(type != ComponentType::COUNT && type != ComponentType::INVALID);

    std::unique_ptr<IComponentArray> componentArray = m_componentArrays[static_cast<std::size_t>(type)];
    if (componentArray == nullptr) {
        WLOG("The component array does not exist!");
        return false;
    }

    m_componentArrays[static_cast<std::size_t>(type)] = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
std::shared_ptr<T> ComponentManager::AddComponent(Entity entity)
{
    //static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");
    assert(entity < INVALID_ENTITY);

    ComponentType type = T::GetType();
    assert(type != ComponentType::COUNT && type != ComponentType::INVALID);

    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[static_cast<std::size_t>(type)])->AddComponent(entity);
}

//----------------------------------------------------------------------------------------------------
template <class T>
bool ComponentManager::RemoveComponent(Entity entity)
{
    //static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");
    assert(entity < INVALID_ENTITY);

    ComponentType type = T::GetType();
    assert(type != ComponentType::COUNT && type != ComponentType::INVALID);

    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[static_cast<std::size_t>(type)])->RemoveComponent(entity);
}

//----------------------------------------------------------------------------------------------------
template <class T>
std::shared_ptr<T> ComponentManager::GetComponent(Entity entity)
{
    //static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");
    assert(entity < INVALID_ENTITY);

    ComponentType type = T::GetType();
    assert(type != ComponentType::COUNT && type != ComponentType::INVALID);

    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[static_cast<std::size_t>(type)])->GetComponent(entity);
}
}

#endif
