#include "SystemManager.h"

#include "EntityManager.h"
#include "Game.h"

#include "Events.h"
#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SystemManager::SystemManager()
    : m_systems()
{
    m_systems.reserve(static_cast<std::size_t>(SystemType::COUNT));
}

//----------------------------------------------------------------------------------------------------
SystemManager::~SystemManager()
{
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::StartUp()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->StartUp();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::PreUpdate(F32 dt)
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->PreUpdate(dt);
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::Update(F32 dt)
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->Update(dt);
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::PostUpdate(F32 dt)
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->PostUpdate(dt);
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::Render()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->Render();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::ShutDown()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->ShutDown();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
void SystemManager::OnNotify(const Event& event)
{
    switch (event.type) {
    case EventType::ENTITY_REMOVED: {
        for (const auto& system : m_systems) {
            system->DeRegisterEntity(event.entity.entity);
        }

        break;
    }

    case EventType::ENTITY_SIGNATURE_CHANGED: {
        for (const auto& system : m_systems) {

            const Signature systemSignature = system->GetSignature();
            const Signature entitySignature = g_game->GetEntityManager().GetSignature(event.entity.entity);
            if (systemSignature == entitySignature) {
                system->RegisterEntity(event.entity.entity);
            } else {
                system->DeRegisterEntity(event.entity.entity);
            }
        }
        break;
    }

    default: {
        break;
    }
    }
}
}
