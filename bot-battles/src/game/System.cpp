#include "System.h"

#include "Log.h"

#include <cassert>

namespace sand {

//----------------------------------------------------------------------------------------------------
System::System()
{
    m_entities.reserve(MAX_ENTITIES);
}

//----------------------------------------------------------------------------------------------------
System::~System()
{
}

//----------------------------------------------------------------------------------------------------
bool System::RegisterEntity(Entity entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {

        LOG("THe entity %u is already registered!", entity);
        return false;
    }

    m_entities.push_back(entity);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool System::DeRegisterEntity(Entity entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end()) {

        LOG("The entity %u is not registered!", entity);
        return false;
    }

    m_entities.erase(it);

    return true;
}
}
