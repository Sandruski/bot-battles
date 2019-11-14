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
    std::vector<std::shared_ptr<System>> m_systems;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::RegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    assert(type != SystemType::COUNT && type != SystemType::INVALID);

    for (const auto& system : m_systems) {
        if (system->GetType() == type) {
            WLOG("The system is already registered!");
            return false;
        }
    }

    m_systems.push_back(std::make_shared<T>());

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::DeRegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    assert(type != SystemType::COUNT && type != SystemType::INVALID);

    for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
        std::shared_ptr<T> system = std::static_pointer_cast<T>(*it);
        if (system->GetType() == type) {
            m_systems.erase(it);
            return true;
        }
    }

    WLOG("The system is not registered!");
    return false;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline std::shared_ptr<T> SystemManager::GetSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    assert(type != SystemType::COUNT && type != SystemType::INVALID);

    for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
        std::shared_ptr<T> system = std::static_pointer_cast<T>(*it);
        if (system->GetType() == type) {
            return system;
        }
    }

    return nullptr;
}
} // namespace sand

#endif
