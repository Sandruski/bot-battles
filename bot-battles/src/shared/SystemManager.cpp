#include "SystemManager.h"

#include "EntityManager.h"

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SystemManager::SystemManager()
    : m_systems()
{
    m_systems.fill(nullptr);
}

//----------------------------------------------------------------------------------------------------
void SystemManager::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::ENTITY_REMOVED: {
        OnEntityRemoved(event.entity.entity);
        break;
    }

    case EventType::ENTITY_SIGNATURE_CHANGED: {
        OnEntitySignatureChanged(event.entity.signature, event.entity.entity);
        break;
    }

    default: {
        break;
    }
    }
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
bool SystemManager::PreUpdate()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->PreUpdate();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::Update()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->Update();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::PostUpdate()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->PostUpdate();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool SystemManager::PreRender()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->PreRender();
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
bool SystemManager::PostRender()
{
    bool ret = true;

    for (U32 i = 0; i < m_systems.size() && ret; ++i) {
        ret = m_systems[i]->PostRender();
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
void SystemManager::OnEntityRemoved(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    for (const auto& system : m_systems) {
        system->DeRegisterEntity(entity);
    }
}

//----------------------------------------------------------------------------------------------------
void SystemManager::OnEntitySignatureChanged(Signature signature, Entity entity)
{
    assert(entity < INVALID_ENTITY);

    for (const auto& system : m_systems) {
        Signature systemSignature = system->GetSignature();
        if ((systemSignature & signature) == systemSignature) {
            system->RegisterEntity(entity);
        } else {
            system->DeRegisterEntity(entity);
        }
    }
}
}
