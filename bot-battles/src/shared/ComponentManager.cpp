#include "ComponentManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ComponentManager::ComponentManager()
    : m_componentArrays()
{
    memset(&m_componentArrays, NULL, static_cast<std::size_t>(ComponentType::COUNT));
}

//----------------------------------------------------------------------------------------------------
ComponentManager::~ComponentManager()
{
}

//----------------------------------------------------------------------------------------------------
bool ComponentManager::PreUpdate(F32 dt)
{
	for (const auto& componentArray : m_componentArrays) {
		componentArray->PreUpdate(dt);
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
