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
void ComponentManager::OnEntityRemoved(Entity entity)
{
    assert(entity < INVALID_ENTITY);

    for (const auto& componentArray : m_componentArrays) {
        componentArray->OnEntityRemoved(entity);
    }
}
}
