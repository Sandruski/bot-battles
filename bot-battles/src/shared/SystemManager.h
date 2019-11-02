#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "SystemDefs.h"

#include "Observer.h"

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

    bool StartUp();
    bool PreUpdate();
    bool Update();
    bool PostUpdate();
    bool Render();
    bool ShutDown();

    void OnNotify(const Event& event);

private:
    std::vector<std::unique_ptr<System>> m_systems;
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

    m_systems.push_back(std::make_unique<T>());

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool SystemManager::DeRegisterSystem()
{
    static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

    SystemType type = T::GetType();
    assert(type != SystemType::COUNT && type != SystemType::INVALID);

    for (const auto& system : m_systems) {
        if (system->GetType() == type) {
            m_systems[static_cast<std::size_t>(type)] = nullptr;
            return true;
        }
    }

    WLOG("The system is not registered!");
    return false;
}
} // namespace sand

#endif
