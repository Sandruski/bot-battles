#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
System::System()
    : m_entities()
    , m_signature()
{
    m_entities.reserve(MAX_ENTITIES);
}

//----------------------------------------------------------------------------------------------------
bool System::RegisterEntity(Entity entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        return false;
    }

    m_entities.emplace_back(entity);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool System::DeRegisterEntity(Entity entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end()) {
        return false;
    }

    m_entities.erase(it);

    return true;
}

//----------------------------------------------------------------------------------------------------
Signature System::GetSignature()
{
    return m_signature;
}
}
