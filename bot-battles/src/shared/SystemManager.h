#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

namespace sand {

class System;

//----------------------------------------------------------------------------------------------------
class SystemManager : public Observer {
public:
    SystemManager();

    void OnNotify(const Event& event) override;

    bool StartUp();
    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool PreRender();
    bool Render();
    bool DebugRender();
    bool PostRender();
    bool ShutDown();

    template <class T>
    bool RegisterSystem();
    template <class T>
    bool DeRegisterSystem();

    template <class T>
    std::weak_ptr<T> GetSystem();

private:
    void OnEntityRemoved(Entity entity);
    void OnEntitySignatureChanged(Signature signature, Entity entity);

private:
    std::array<std::shared_ptr<System>, MAX_SYSTEMS> m_systems;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::RegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType systemType = T::GetType();
    assert(systemType < SystemType::COUNT);
    std::size_t systemIndex = static_cast<std::size_t>(systemType);
    std::shared_ptr<System> system = m_systems.at(systemIndex);
    if (system != nullptr) {
        WLOG("System %u is already registered!", systemIndex);
        return false;
    }

    m_systems.at(systemIndex) = std::make_shared<T>();

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::DeRegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType systemType = T::GetType();
    assert(systemType < SystemType::COUNT);
    std::size_t systemIndex = static_cast<std::size_t>(systemType);
    std::shared_ptr<System> system = m_systems.at(systemIndex);
    if (system == nullptr) {
        WLOG("System %u is not registered!", systemIndex);
        return false;
    }

    m_systems.at(systemIndex) = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::weak_ptr<T> SystemManager::GetSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType systemType = T::GetType();
    assert(systemType < SystemType::COUNT);
    std::size_t systemIndex = static_cast<std::size_t>(systemType);
    std::shared_ptr<System> system = m_systems.at(systemIndex);
    return std::weak_ptr<T>(std::static_pointer_cast<T>(system));
}
}

#endif
