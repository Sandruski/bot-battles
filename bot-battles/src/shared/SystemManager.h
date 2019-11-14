#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

namespace sand {

class System;

//----------------------------------------------------------------------------------------------------
class SystemManager : public Observer {
public:
    SystemManager();
    ~SystemManager();

    template <class T>
    bool RegisterSystem();
    template <class T>
    bool DeRegisterSystem();
    template <class T>
    std::shared_ptr<T> GetSystem();

    bool StartUp();
    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool Render();
    bool ShutDown();

    void OnNotify(const Event& event);

private:
    std::array<std::shared_ptr<System>, MAX_SYSTEMS> m_systems;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::RegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    std::size_t index = static_cast<std::size_t>(type);
    std::shared_ptr<System> system = m_systems.at(index);
    if (system != nullptr) {
        WLOG("The system is already registered!");
        return false;
    }

    m_systems[index] = std::make_shared<T>();

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::DeRegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    std::size_t index = static_cast<std::size_t>(type);
    std::shared_ptr<System> system = m_systems.at(index);
    if (system == nullptr) {
        WLOG("The system is not registered!");
        return false;
    }

    m_systems[index] = nullptr;

    return false;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::shared_ptr<T> SystemManager::GetSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    std::size_t index = static_cast<std::size_t>(type);
    std::shared_ptr<System> system = m_systems.at(index);
    return std::static_pointer_cast<T>(system);
}
} // namespace sand

#endif
