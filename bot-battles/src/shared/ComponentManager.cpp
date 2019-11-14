#include "ComponentManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ComponentManager::ComponentManager()
    : m_componentArrays()
{
    m_componentArrays.fill(NULL);
}

//----------------------------------------------------------------------------------------------------
ComponentManager::~ComponentManager()
{
}

//----------------------------------------------------------------------------------------------------
bool ComponentManager::PreUpdate()
{
    for (const auto& componentArray : m_componentArrays) {
        componentArray->PreUpdate();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ComponentManager::AddObserver(std::shared_ptr<Observer> observer)
{
    for (const auto& componentArray : m_componentArrays) {
        componentArray->AddObserver(observer);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ComponentManager::RemoveObserver(std::shared_ptr<Observer> observer)
{
    for (const auto& componentArray : m_componentArrays) {
        componentArray->RemoveObserver(observer);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void ComponentManager::OnNotify(const Event& event)
{
    for (const auto& componentArray : m_componentArrays) {
        componentArray->OnNotify(event);
    }
}
}
